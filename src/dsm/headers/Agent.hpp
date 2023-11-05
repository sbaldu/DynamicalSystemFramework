/// @file       src/Agent.hpp
/// @brief      Defines the Agent class.
///
/// @details    This file contains the definition of the Agent class.
///             The Agent class represents an agent in the network. It is templated by the type
///             of the agent's id. The agent's id must be an unsigned integral type.

#ifndef Agent_hpp
#define Agent_hpp

#include "Itinerary.hpp"
#include "SparseMatrix.hpp"
#include "../utility/TypeTraits/is_numeric.hpp"

#include <stdexcept>
#include <string>
#include <limits>

namespace dsm {
  /// @brief The Agent class represents an agent in the network.
  /// @tparam Id The type of the agent's id. It must be an unsigned integral type.
  template <typename Id>
    requires std::unsigned_integral<Id>
  class Agent {
  private:
    Itinerary<Id> m_itinerary;
    double m_speed;
    double m_position;
    Id m_index;
    Id m_street;
    unsigned int m_time;

  public:
    Agent() = default;
    /// @brief Construct a new Agent object
    /// @param index, The agent's id
    /// @param street, The agent's street
    Agent(Id index, Id street);
    /// @brief Construct a new Agent object
    /// @param index, The agent's id
    /// @param street, The agent's street
    /// @param itinerary, The agent's itinerary
    Agent(Id index, Id street, Itinerary<Id> itinerary);

    /// @brief Set the agent's street
    /// @param street, The agent's street
    void setStreet(Id street);
    /// @brief Set the agent's position
    /// @param position, The agent's position
    void setPosition(double position);
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
    int index() const;
    /// @brief Get the agent's street
    /// @return The agent's street
    int street() const;
    /// @brief Get the agent's position
    /// @return The agent's position
    int position() const;
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
  Agent<Id>::Agent(Id index, Id street)
      : m_speed{0.}, m_position{0.}, m_index{index}, m_street{street}, m_time{0} {}

  template <typename Id>
    requires std::unsigned_integral<Id>
  Agent<Id>::Agent(Id index, Id street, Itinerary<Id> itinerary)
      : m_itinerary{std::move(itinerary)},
        m_speed{0.},
        m_position{0.},
        m_index{index},
        m_street{street},
        m_time{0} {}

  template <typename Id>
    requires std::unsigned_integral<Id>
  void Agent<Id>::setStreet(Id street) {
    m_street = street;
  }
  template <typename Id>
    requires std::unsigned_integral<Id>
  void Agent<Id>::setPosition(double position) {
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

  template <typename Id>
    requires std::unsigned_integral<Id>
  int Agent<Id>::index() const {
    return m_index;
  }
  template <typename Id>
    requires std::unsigned_integral<Id>
  int Agent<Id>::street() const {
    return m_street;
  }
  template <typename Id>
    requires std::unsigned_integral<Id>
  int Agent<Id>::position() const {
    return m_position;
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
};  // namespace dsm

#endif
