/// @file       src/Agent.hpp
/// @brief      Defines the Agent class.
///
/// @details    This file contains the definition of the Agent class.
///             The Agent class represents an agent in the network. It is templated by the type
///             of the agent's id and the size of agents, which must both be unsigned integrals.
///				      It is also templated by the Delay type, which must be a numeric (see utility/TypeTraits/is_numeric.hpp)
///				      and represents the spatial or temporal (depending on the type of the template) distance
///				      between the agent and the one in front of it.

#ifndef Agent_hpp
#define Agent_hpp

#include "Itinerary.hpp"
#include "SparseMatrix.hpp"
#include "../utility/TypeTraits/is_numeric.hpp"

#include <concepts>
#include <stdexcept>
#include <string>
#include <limits>

namespace dsm {
  /// @brief The Agent class represents an agent in the network.
  /// @tparam Id, The type of the agent's id. It must be an unsigned integral type.
  /// @tparam Size, The type of the size of a street. It must be an unsigned integral type.
  /// @tparam Delay, The type of the agent's delay. It must be a numeric type (see utility/TypeTraits/is_numeric.hpp).
  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> && is_numeric_v<Delay>
  class Agent {
  private:
    Id m_id;
    Id m_streetId;
    Id m_itineraryId;
    Delay m_delay;
    double m_speed;
    unsigned int m_time;

  public:
    Agent() = delete;
    /// @brief Construct a new Agent object
    /// @param id The agent's id
    /// @param streetId The id of the street currently occupied by the agent
    /// @param itineraryId The agent's itinerary
    Agent(Id id, Id streetId, Id itineraryId);
    /// @brief Set the street occupied by the agent
    /// @param streetId The id of the street currently occupied by the agent
    void setStreetId(Id streetId);
    /// @brief Set the agent's itinerary
    /// @param itineraryId The agent's itinerary
    void setItineraryId(Id itineraryId);
    /// @brief Set the agent's speed
    /// @param speed, The agent's speed
    /// @throw std::invalid_argument, if speed is negative
    void setSpeed(double speed);
    /// @brief Increment the agent's delay by 1
    /// @throw std::overflow_error, if delay has reached its maximum value
    void incrementDelay();
    /// @brief Set the agent's delay
    /// @param delay The agent's delay
    /// @throw std::overflow_error, if delay has reached its maximum value
    void incrementDelay(Delay delay);
    /// @brief Decrement the agent's delay by 1
    /// @throw std::underflow_error, if delay has reached its minimum value
    void decrementDelay();
    /// @brief Increment the agent's time by 1
    /// @throw std::overflow_error, if time has reached its maximum value
    void incrementTime();
    /// @brief Increment the agent's time by a given value
    /// @param time The value to increment the agent's time by
    /// @throw std::overflow_error, if time has reached its maximum value
    void incrementTime(unsigned int time);
    /// @brief Reset the agent's time to 0
    void resetTime();

    /// @brief Get the agent's id
    /// @return The agent's id
    Id id() const;
    /// @brief Get the id of the street currently occupied by the agent
    /// @return The id of the street currently occupied by the agent
    Id streetId() const;
    /// @brief Get the agent's itinerary
    /// @return The agent's itinerary
    Id itineraryId() const;
    /// @brief Get the agent's speed
    /// @return The agent's speed
    double speed() const;
    /// @brief Get the agent's delay
    /// @return	The agent's delay
    Delay delay() const;
    /// @brief Get the agent's travel time
    /// @return The agent's travel time
    unsigned int time() const;
  };

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> && is_numeric_v<Delay>)
  Agent<Id, Size, Delay>::Agent(Id id, Id streetId, Id itineraryId)
      : m_id{id}, m_streetId{streetId}, m_itineraryId{itineraryId}, m_delay{0}, m_speed{0.}, m_time{0} {}

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> && is_numeric_v<Delay>)
  void Agent<Id, Size, Delay>::setStreetId(Id streetId) {
    m_streetId = streetId;
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> && is_numeric_v<Delay>)
  void Agent<Id, Size, Delay>::setItineraryId(Id itineraryId) {
    m_itineraryId = itineraryId;
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> && is_numeric_v<Delay>)
  void Agent<Id, Size, Delay>::setSpeed(double speed) {
    if (speed < 0) {
      std::string errorMsg{"Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
                           "Speed must be positive"};
      throw std::invalid_argument(errorMsg);
    }
    m_speed = speed;
  }
  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> && is_numeric_v<Delay>)
  void Agent<Id, Size, Delay>::incrementDelay() {
    if (m_delay == std::numeric_limits<Delay>::max()) {
      std::string errorMsg{"Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
                           "Delay has reached its maximum value"};
      throw std::overflow_error(errorMsg);
    }
    ++m_delay;
  }
  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> && is_numeric_v<Delay>)
  void Agent<Id, Size, Delay>::incrementDelay(Delay delay) {
    if (m_delay + delay < m_delay) {
      std::string errorMsg{"Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
                           "Delay has reached its maximum value"};
      throw std::overflow_error(errorMsg);
    }
    m_delay = delay;
  }
  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> && is_numeric_v<Delay>)
  void Agent<Id, Size, Delay>::decrementDelay() {
    if (m_delay == 0) {
      std::string errorMsg{"Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
                           "Delay has reached its minimum value"};
      throw std::underflow_error(errorMsg);
    }
    --m_delay;
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> && is_numeric_v<Delay>)
  void Agent<Id, Size, Delay>::incrementTime() {
    if (m_time == std::numeric_limits<unsigned int>::max()) {
      std::string errorMsg{"Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
                           "Time has reached its maximum value"};
      throw std::overflow_error(errorMsg);
    }
    ++m_time;
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> && is_numeric_v<Delay>)
  void Agent<Id, Size, Delay>::incrementTime(unsigned int time) {
    if (m_time + time < m_time) {
      std::string errorMsg{"Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
                           "Time has reached its maximum value"};
      throw std::overflow_error(errorMsg);
    }
    m_time += time;
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> && is_numeric_v<Delay>)
  Id Agent<Id, Size, Delay>::id() const {
    return m_id;
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> && is_numeric_v<Delay>)
  Id Agent<Id, Size, Delay>::streetId() const {
    return m_streetId;
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> && is_numeric_v<Delay>)
  double Agent<Id, Size, Delay>::speed() const {
    return m_speed;
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> && is_numeric_v<Delay>)
  Delay Agent<Id, Size, Delay>::delay() const {
    return m_delay;
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> && is_numeric_v<Delay>)
  unsigned int Agent<Id, Size, Delay>::time() const {
    return m_time;
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> && is_numeric_v<Delay>)
  Id Agent<Id, Size, Delay>::itineraryId() const {
    return m_itineraryId;
  }
};  // namespace dsm

#endif
