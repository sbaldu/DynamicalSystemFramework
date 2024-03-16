/// @file       /src/dsm/headers/Agent.hpp
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

include "Itinerary.hpp"
#include "SparseMatrix.hpp"
#include "../utility/TypeTraits/is_numeric.hpp"
#include "../utility/Logger.hpp"

#include <concepts>
#include <stdexcept>
#include <limits>
#include <optional>

    namespace dsm {
  /// @brief The Agent class represents an agent in the network.
  /// @tparam Id, The type of the agent's id. It must be an unsigned integral type.
  /// @tparam Size, The type of the size of a street. It must be an unsigned integral type.
  /// @tparam Delay, The type of the agent's delay. It must be a numeric type (see utility/TypeTraits/is_numeric.hpp).
  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  class Agent {
  private:
    Id m_id;
    Id m_itineraryId;
    std::optional<Id> m_streetId;
    std::optional<Id> m_srcNodeId;
    Delay m_delay;
    double m_speed;
    double m_distance;    // Travelled distance
    unsigned int m_time;  // Travelled time

  public:
    Agent() = delete;
    /// @brief Construct a new Agent object
    /// @param id The agent's id
    /// @param itineraryId The agent's itinerary
    Agent(Id id, Id itineraryId);
    /// @brief Construct a new Agent object
    /// @param id The agent's id
    /// @param itineraryId The agent's itinerary
    /// @param srcNodeId The id of the source node of the agent
    Agent(Id id, Id itineraryId, Id srcNodeId);
    /// @brief Set the street occupied by the agent
    /// @param streetId The id of the street currently occupied by the agent
    void setStreetId(Id streetId);
    /// @brief Set the source node id of the agent
    /// @param srcNodeId The id of the source node of the agent
    void setSourceNodeId(Id srcNodeId);
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
    /// @brief Increment the agent's distance by its speed * 1 second
    void incrementDistance();
    /// @brief Increment the agent's distance by a given value
    /// @param distance The value to increment the agent's distance byù
    /// @throw std::invalid_argument, if distance is negative
    void incrementDistance(double distance);
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
    /// @brief Get the agent's itinerary
    /// @return The agent's itinerary
    Id itineraryId() const;
    /// @brief Get the id of the street currently occupied by the agent
    /// @return The id of the street currently occupied by the agent
    std::optional<Id> streetId() const;
    /// @brief Get the id of the source node of the agent
    /// @return The id of the source node of the agent
    std::optional<Id> srcNodeId() const;
    /// @brief Get the agent's speed
    /// @return The agent's speed
    double speed() const;
    /// @brief Get the agent's delay
    /// @return	The agent's delay
    Delay delay() const;
    /// @brief Get the agent's travelled distance
    /// @return The agent's travelled distance
    double distance() const;
    /// @brief Get the agent's travel time
    /// @return The agent's travel time
    unsigned int time() const;
  };

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  Agent<Id, Size, Delay>::Agent(Id id, Id itineraryId)
      : m_id{id},
        m_itineraryId{itineraryId},
        m_delay{0},
        m_speed{0.},
        m_distance{0.},
        m_time{0} {}

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  Agent<Id, Size, Delay>::Agent(Id id, Id itineraryId, Id srcNodeId)
      : m_id{id},
        m_itineraryId{itineraryId},
        m_srcNodeId{srcNodeId},
        m_delay{0},
        m_speed{0.},
        m_distance{0.},
        m_time{0} {}

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  void Agent<Id, Size, Delay>::setStreetId(Id streetId) {
    m_streetId = streetId;
  }
  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  void Agent<Id, Size, Delay>::setSourceNodeId(Id srcNodeId) {
    m_srcNodeId = srcNodeId;
  }
  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  void Agent<Id, Size, Delay>::setItineraryId(Id itineraryId) {
    m_itineraryId = itineraryId;
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  void Agent<Id, Size, Delay>::setSpeed(double speed) {
    if (speed < 0) {
      throw std::invalid_argument(buildLog("Speed must be positive"));
    }
    m_speed = speed;
  }
  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  void Agent<Id, Size, Delay>::incrementDelay() {
    if (m_delay == std::numeric_limits<Delay>::max()) {
      throw std::overflow_error(buildLog("Delay has reached its maximum value"));
    }
    ++m_delay;
  }
  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  void Agent<Id, Size, Delay>::incrementDelay(Delay delay) {
    if (m_delay + delay < m_delay) {
      throw std::overflow_error(buildLog("Delay has reached its maximum value"));
    }
    m_delay = delay;
  }
  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  void Agent<Id, Size, Delay>::decrementDelay() {
    if (m_delay == 0) {
      throw std::underflow_error(buildLog("Delay has reached its minimum value"));
    }
    --m_delay;
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  void Agent<Id, Size, Delay>::incrementDistance() {
    m_distance += m_speed;  // actually m_speed * 1 second
  }
  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  void Agent<Id, Size, Delay>::incrementDistance(double distance) {
    if (distance < 0) {
      throw std::invalid_argument(buildLog("Distance travelled must be positive"));
    }
    m_distance += distance;
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  void Agent<Id, Size, Delay>::incrementTime() {
    if (m_time == std::numeric_limits<unsigned int>::max()) {
      throw std::overflow_error(buildLog("Time has reached its maximum value"));
    }
    ++m_time;
  }
  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  void Agent<Id, Size, Delay>::incrementTime(unsigned int time) {
    if (m_time + time < m_time) {
      throw std::overflow_error(buildLog("Time has reached its maximum value"));
    }
    m_time += time;
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  Id Agent<Id, Size, Delay>::id() const {
    return m_id;
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  std::optional<Id> Agent<Id, Size, Delay>::streetId() const {
    return m_streetId;
  }
  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  std::optional<Id> Agent<Id, Size, Delay>::srcNodeId() const {
    return m_srcNodeId;
  }
  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  double Agent<Id, Size, Delay>::speed() const {
    return m_speed;
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  Delay Agent<Id, Size, Delay>::delay() const {
    return m_delay;
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  double Agent<Id, Size, Delay>::distance() const {
    return m_distance;
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  unsigned int Agent<Id, Size, Delay>::time() const {
    return m_time;
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  Id Agent<Id, Size, Delay>::itineraryId() const {
    return m_itineraryId;
  }
};  // namespace dsm

#endif
