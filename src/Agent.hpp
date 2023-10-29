/// @file       src/Agent.hpp
/// @brief      Defines the Agent class.
///
/// @details    This file contains the definition of the Agent class.
///             The Agent class represents an agent in the network. It is templated by the type of the agent's id. The
///             agent's id must be an unsigned integral type.

#ifndef Agent_hpp
#define Agent_hpp

#include "Itinerary.hpp"

#include <stdexcept>
#include <string>
#include <limits>

namespace dmf {

  template <typename Id>
    requires std::unsigned_integral<Id>
  class Agent {
  private:
    Itinerary<Id> m_itinerary;
    double m_speed;
    Id m_index;
    Id m_position;
    Id m_previousPosition;
    unsigned int m_time;

  public:
    Agent() = default;
    /// @brief Construct a new Agent<Id>:: Agent object
    /// @param index The agent's id
    /// @param position The agent's position
    Agent(Id index, Id position);
    /// @brief Construct a new Agent<Id>:: Agent object
    /// @param index The agent's id
    /// @param position The agent's position
    /// @param itinerary The agent's itinerary
    Agent(Id index, Id position, Itinerary<Id> itinerary);

    /*************************************************************************/
    /* Setters                                                               */
    /*************************************************************************/
    /// @brief Set the agent's position
    /// @param position The agent's position
    void setPosition(Id position);
    /// @brief Set the agent's itinerary
    /// @param itinerary The agent's itinerary
    void setItinerary(Itinerary<Id> itinerary);
    /// @brief Set the agent's speed
    /// @param speed The agent's speed
    /// @throw std::invalid_argument if speed is negative
    void setSpeed(double speed);
    /// @brief Increment the agent's time by 1
    /// @throw std::overflow_error if time has reached its maximum value
    void incrementTime();
    /// @brief Increment the agent's time by a given value
    /// @param time The value to increment the agent's time by
    /// @throw std::overflow_error if time has reached its maximum value
    void incrementTime(unsigned int time);
    /// @brief Reset the agent's time to 0
    void resetTime();

    /*************************************************************************/
    /* Getters                                                               */
    /*************************************************************************/
    /// @brief Get the agent's id
    /// @return The agent's id
    int index() const;
    /// @brief Get the agent's position
    /// @return The agent's position
    int position() const;
    /// @brief Get the agent's previous position
    /// @return The agent's previous position
    int previousPosition() const;
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

  template <typename Id>
    requires std::unsigned_integral<Id>
  Agent<Id>::Agent(Id index, Id position)
      : m_speed{0.}, m_index{index}, m_position{position}, m_previousPosition{position}, m_time{0} {}

  template <typename Id>
    requires std::unsigned_integral<Id>
  Agent<Id>::Agent(Id index, Id position, Itinerary<Id> itinerary)
      : m_itinerary{std::move(itinerary)},
        m_speed{0.},
        m_index{index},
        m_position{position},
        m_previousPosition{position},
        m_time{0} {}

  // Setters
  template <typename Id>
    requires std::unsigned_integral<Id>
  void Agent<Id>::setPosition(Id position) {
    m_position = position;
  }
  template <typename Id>
    requires std::unsigned_integral<Id>
  void Agent<Id>::setItinerary(Itinerary<Id> itinerary) {
    m_itinerary = std::move(itinerary);
  }
  template <typename Id>
    requires std::unsigned_integral<Id>
  void Agent<Id>::setSpeed(double speed) {
    if (speed < 0) {
      std::string errorMsg = "Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
                             "Speed must be positive";
      throw std::invalid_argument(errorMsg);
    }
    m_speed = speed;
  }
  template <typename Id>
    requires std::unsigned_integral<Id>
  void Agent<Id>::incrementTime() {
    if (m_time == std::numeric_limits<unsigned int>::max()) {
      std::string errorMsg = "Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
                             "Time has reached its maximum value";
      throw std::overflow_error(errorMsg);
    }
    ++m_time;
  }
  template <typename Id>
    requires std::unsigned_integral<Id>
  void Agent<Id>::incrementTime(unsigned int time) {
    if (m_time + time < m_time) {
      std::string errorMsg = "Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
                             "Time has reached its maximum value";
      throw std::overflow_error(errorMsg);
    }
    m_time += time;
  }

  // Getters
  template <typename Id>
    requires std::unsigned_integral<Id>
  int Agent<Id>::index() const {
    return m_index;
  }
  template <typename Id>
    requires std::unsigned_integral<Id>
  int Agent<Id>::position() const {
    return m_position;
  }
  template <typename Id>
    requires std::unsigned_integral<Id>
  int Agent<Id>::previousPosition() const {
    return m_previousPosition;
  }
  template <typename Id>
    requires std::unsigned_integral<Id>
  double Agent<Id>::speed() const {
    return m_speed;
  }
  template <typename Id>
    requires std::unsigned_integral<Id>
  unsigned int Agent<Id>::time() const {
    return m_time;
  }
  template <typename Id>
    requires std::unsigned_integral<Id>
  const Itinerary<Id>& Agent<Id>::itinerary() const {
    return m_itinerary;
  }
};  // namespace dmf

#endif
