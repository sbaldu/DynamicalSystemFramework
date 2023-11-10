/// @file       src/Agent.hpp
/// @brief      Defines the Agent class.
///
/// @details    This file contains the definition of the Agent class.
///             The Agent class represents an agent in the network. It is templated by the type
///             of the agent's id and the size of agents, which must both be unsigned integrals.

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
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  class Agent {
  private:
    Itinerary<Id> m_itinerary;
    double m_speed;
    Id m_index;
    Id m_streetId;
    unsigned int m_time;

  public:
    Agent() = default;
    /// @brief Construct a new Agent object
    /// @param index, The agent's id
    /// @param streetId, The id of the street currently occupied by the agent
    Agent(Id index, Id streetId);
    /// @brief Construct a new Agent object
    /// @param index, The agent's id
    /// @param streetId, The id of the street currently occupied by the agent
    /// @param itinerary, The agent's itinerary
    Agent(Id index, Id streetId, Itinerary<Id> itinerary);

    /// @brief Set the street occupied by the agent
    /// @param streetId, The id of the street currently occupied by the agent
    void setPosition(Id streetId);
    /// @brief Set the agent's itinerary
    /// @param itinerary, The agent's itinerary
    void setItinerary(Itinerary<Id> itinerary);
    /// @brief Set the agent's speed
    /// @param speed, The agent's speed
    /// @throw std::invalid_argument, if speed is negative
    void setSpeed(double speed);
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
    /// @brief Get the agent's itinerary
    /// @return The agent's itinerary
    const Itinerary<Id>& itinerary() const;
    /// @brief Get the agent's speed
    /// @return The agent's speed
    double speed() const;
    /// @brief Get the agent's travel time
    /// @return The agent's travel time
    unsigned int time() const;
  };

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  Agent<Id, Size>::Agent(Id index, Id streetId)
      : m_speed{0.}, m_index{index}, m_streetId{streetId}, m_time{0} {}

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  Agent<Id, Size>::Agent(Id index, Id streetId, Itinerary<Id> itinerary)
      : m_itinerary{std::move(itinerary)},
        m_speed{0.},
        m_index{index},
        m_streetId{streetId},
        m_time{0} {}

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Agent<Id, Size>::setPosition(Id streetId) {
    m_streetId = streetId;
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Agent<Id, Size>::setItinerary(Itinerary<Id> itinerary) {
    m_itinerary = std::move(itinerary);
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Agent<Id, Size>::setSpeed(double speed) {
    if (speed < 0) {
      std::string errorMsg = "Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
                             "Speed must be positive";
      throw std::invalid_argument(errorMsg);
    }
    m_speed = speed;
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Agent<Id, Size>::incrementTime() {
    if (m_time == std::numeric_limits<unsigned int>::max()) {
      std::string errorMsg = "Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
                             "Time has reached its maximum value";
      throw std::overflow_error(errorMsg);
    }
    ++m_time;
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Agent<Id, Size>::incrementTime(unsigned int time) {
    if (m_time + time < m_time) {
      std::string errorMsg = "Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
                             "Time has reached its maximum value";
      throw std::overflow_error(errorMsg);
    }
    m_time += time;
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  Id Agent<Id, Size>::index() const {
    return m_index;
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  Id Agent<Id, Size>::streetId() const {
    return m_streetId;
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  double Agent<Id, Size>::speed() const {
    return m_speed;
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  unsigned int Agent<Id, Size>::time() const {
    return m_time;
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  const Itinerary<Id>& Agent<Id, Size>::itinerary() const {
    return m_itinerary;
  }
};  // namespace dsm

#endif
