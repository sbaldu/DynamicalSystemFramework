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
    Itinerary<Id> m_itinerary;
    double m_speed;
    Id m_index;
    Id m_streetId;
    Id m_nextNodeId;
    Delay m_delay;
    unsigned int m_time;

  public:
    Agent() = delete;
    /// @brief Construct a new Agent object
    /// @param index The agent's id
    /// @param streetId The id of the street currently occupied by the agent
    /// @param nextNodeId The id of the node to which the agent is heading
    Agent(Id index, Id streetId, Id nextNodeId);
    /// @brief Construct a new Agent object
    /// @param index The agent's id
    /// @param streetId The id of the street currently occupied by the agent
    /// @param itinerary The agent's itinerary
    Agent(Id index, Id streetId, Itinerary<Id> itinerary);

    /// @brief Set the street occupied by the agent
    /// @param streetId, The id of the street currently occupied by the agent
    void setStreetId(Id streetId);
    /// @brief Set the id of the next node in the agent's itinerary
    /// @param nextNodeId The id of the next node in the agent's itinerary
    void setNextNodeId(Id nextNodeId);
    /// @brief Set the agent's itinerary
    /// @param itinerary, The agent's itinerary
    void setItinerary(Itinerary<Id> itinerary);
    /// @brief Set the agent's speed
    /// @param speed, The agent's speed
    /// @throw std::invalid_argument, if speed is negative
    void setSpeed(double speed);
    /// @brief Set the agent's delay
    /// @param delay, The agent's delay
    void setDelay(Delay delay);
    /// @brief Increment the agent's time by 1
    /// @throw std::overflow_error, if time has reached its maximum value
    void incrementTime();
    /// @brief Increment the agent's time by a given value
    /// @param time, The value to increment the agent's time by
    /// @throw std::overflow_error, if time has reached its maximum value
    void incrementTime(unsigned int time);
    /// @brief Reset the agent's time to 0
    void resetTime();

    /// @brief Get the agent's id
    /// @return The agent's id
    Id index() const;
    /// @brief Get the id of the street currently occupied by the agent
    /// @return The id of the street currently occupied by the agent
    Id streetId() const;
    /// @brief Get the id of the node to which the agent is heading
    /// @return The id of the node to which the agent is heading
    Id nextNodeId() const;
    /// @brief Get the agent's itinerary
    /// @return The agent's itinerary
    const Itinerary<Id>& itinerary() const;
    /// @brief Get the agent's speed
    /// @return The agent's speed
    double speed() const;
    /// @brief Get the agent's delay
    /// @return	The agent's delay
    Delay delay() const;
    /// @brief Get the agent's travel time
    /// @return The agent's travel time
    unsigned int time() const;
    /// @brief Check if the agent has arrived at its destination
    /// @return true, if the agent has arrived at its destination
    bool has_arrived() const;

    /// @brief Increment the agent's delay by 1
    /// @throw std::overflow_error, if delay has reached its maximum value
    Agent& operator++();
    /// @brief Decrement the agent's delay by 1
    /// @throw std::underflow_error, if delay has reached its minimum value
    Agent& operator--();
  };

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> && is_numeric_v<Delay>)
  Agent<Id, Size, Delay>::Agent(Id index, Id streetId, Id nextNodeId)
      : m_speed{0.}, m_index{index}, m_streetId{streetId}, m_nextNodeId{nextNodeId}, m_delay{0}, m_time{0} {}

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> && is_numeric_v<Delay>)
  Agent<Id, Size, Delay>::Agent(Id index, Id streetId, Itinerary<Id> itinerary)
      : m_itinerary{std::move(itinerary)},
        m_speed{0.},
        m_index{index},
        m_streetId{streetId},
        m_nextNodeId{itinerary.source()},
        m_delay{0},
        m_time{0} {}

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> && is_numeric_v<Delay>)
  void Agent<Id, Size, Delay>::setStreetId(Id streetId) {
    m_streetId = streetId;
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> && is_numeric_v<Delay>)
  void Agent<Id, Size, Delay>::setNextNodeId(Id nextNodeId) {
    m_nextNodeId = nextNodeId;
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> && is_numeric_v<Delay>)
  void Agent<Id, Size, Delay>::setItinerary(Itinerary<Id> itinerary) {
    m_itinerary = std::move(itinerary);
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
  void Agent<Id, Size, Delay>::setDelay(Delay delay) {
    m_delay = delay;
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
  Id Agent<Id, Size, Delay>::index() const {
    return m_index;
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> && is_numeric_v<Delay>)
  Id Agent<Id, Size, Delay>::streetId() const {
    return m_streetId;
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> && is_numeric_v<Delay>)
  Id Agent<Id, Size, Delay>::nextNodeId() const {
    return m_nextNodeId;
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
  const Itinerary<Id>& Agent<Id, Size, Delay>::itinerary() const {
    return m_itinerary;
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> && is_numeric_v<Delay>) bool
  Agent<Id, Size, Delay>::has_arrived() const {
    return (m_delay == 0 && m_nextNodeId == m_itinerary.destination());
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> && is_numeric_v<Delay>)
  Agent<Id, Size, Delay>& Agent<Id, Size, Delay>::operator++() {
    if (m_delay == std::numeric_limits<Delay>::max()) {
      std::string errorMsg{"Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
                           "Delay has reached its maximum value"};
      throw std::overflow_error(errorMsg);
    }
    ++m_delay;
    return *this;
  }
  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> && is_numeric_v<Delay>)
  Agent<Id, Size, Delay>& Agent<Id, Size, Delay>::operator--() {
    if (m_delay == 0) {
      std::string errorMsg{"Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
                           "Delay has reached its minimum value"};
      throw std::underflow_error(errorMsg);
    }
    --m_delay;
    return *this;
  }
};  // namespace dsm

#endif
