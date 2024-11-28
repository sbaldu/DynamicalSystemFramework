/// @file       /src/dsm/headers/Agent.hpp
/// @brief      Defines the Agent class.
///
/// @details    This file contains the definition of the Agent class.
///             The Agent class represents an agent in the network. It is templated by the type
///             of the agent's id and the size of agents, which must both be unsigned integrals.
///				      It is also templated by the Delay type, which must be a numeric (see utility/TypeTraits/is_numeric.hpp)
///				      and represents the spatial or temporal (depending on the type of the template) distance
///				      between the agent and the one in front of it.

#pragma once

#include "Itinerary.hpp"
#include "SparseMatrix.hpp"
#include "../utility/TypeTraits/is_numeric.hpp"
#include "../utility/Logger.hpp"
#include "../utility/Typedef.hpp"

#include <concepts>
#include <stdexcept>
#include <limits>
#include <optional>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace dsm {
  /// @brief The Agent class represents an agent in the network.
  /// @tparam Delay, The type of the agent's delay. It must be a numeric type (see utility/TypeTraits/is_numeric.hpp).
  template <typename Delay>
    requires(is_numeric_v<Delay>)
  class Agent {
  private:
    inline static auto const pConsoleLogger{spdlog::stdout_color_mt("DSM_AGENT_CONSOLE")};
    Id m_id;
    Id m_itineraryId;
    std::optional<Id> m_streetId;
    std::optional<Id> m_srcNodeId;
    Delay m_delay;
    double m_speed;
    double m_distance;    // Travelled distance
    unsigned int m_time;  // Travelled time

  public:
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
    void setStreetId(Id streetId) { m_streetId = streetId; }
    /// @brief Set the source node id of the agent
    /// @param srcNodeId The id of the source node of the agent
    void setSourceNodeId(Id srcNodeId) { m_srcNodeId = srcNodeId; }
    /// @brief Set the agent's itinerary
    /// @param itineraryId The agent's itinerary
    void setItineraryId(Id itineraryId) { m_itineraryId = itineraryId; }
    /// @brief Set the agent's speed
    /// @param speed, The agent's speed
    /// @throw std::invalid_argument, if speed is negative
    void setSpeed(double speed);
    /// @brief Increment the agent's delay by 1
    void incrementDelay();
    /// @brief Increment the agent's delay by a given value
    /// @param delay The agent's delay
    void incrementDelay(Delay const delay);
    /// @brief Decrement the agent's delay by 1
    /// @throw std::underflow_error, if delay has reached its minimum value
    void decrementDelay();
    /// @brief Increment the agent's distance by its speed * 1 second
    void incrementDistance() { m_distance += m_speed; }
    /// @brief Increment the agent's distance by a given value
    /// @param distance The value to increment the agent's distance byù
    /// @throw std::invalid_argument, if distance is negative
    void incrementDistance(double distance);
    /// @brief Increment the agent's time by 1.
    void incrementTime();
    /// @brief Increment the agent's time by a given value.
    /// @param time The value to increment the agent's time by
    void incrementTime(unsigned int const time);
    /// @brief Reset the agent's time to 0
    void resetTime() { m_time = 0; }

    /// @brief Get the agent's id
    /// @return The agent's id
    Id id() const { return m_id; }
    /// @brief Get the agent's itinerary
    /// @return The agent's itinerary
    Id itineraryId() const { return m_itineraryId; }
    /// @brief Get the id of the street currently occupied by the agent
    /// @return The id of the street currently occupied by the agent
    std::optional<Id> streetId() const { return m_streetId; }
    /// @brief Get the id of the source node of the agent
    /// @return The id of the source node of the agent
    std::optional<Id> srcNodeId() const { return m_srcNodeId; }
    /// @brief Get the agent's speed
    /// @return The agent's speed
    double speed() const { return m_speed; }
    /// @brief Get the agent's delay
    /// @return	The agent's delay
    Delay delay() const { return m_delay; }
    /// @brief Get the agent's travelled distance
    /// @return The agent's travelled distance
    double distance() const { return m_distance; }
    /// @brief Get the agent's travel time
    /// @return The agent's travel time
    unsigned int time() const { return m_time; }
  };

  template <typename Delay>
    requires(is_numeric_v<Delay>)
  Agent<Delay>::Agent(Id id, Id itineraryId)
      : m_id{id},
        m_itineraryId{itineraryId},
        m_delay{0},
        m_speed{0.},
        m_distance{0.},
        m_time{0} {}

  template <typename Delay>
    requires(is_numeric_v<Delay>)
  Agent<Delay>::Agent(Id id, Id itineraryId, Id srcNodeId)
      : m_id{id},
        m_itineraryId{itineraryId},
        m_srcNodeId{srcNodeId},
        m_delay{0},
        m_speed{0.},
        m_distance{0.},
        m_time{0} {}

  template <typename Delay>
    requires(is_numeric_v<Delay>)
  void Agent<Delay>::setSpeed(double speed) {
    if (speed < 0) {
      throw std::invalid_argument(buildLog("Speed must be positive"));
    }
    m_speed = speed;
  }
  template <typename Delay>
    requires(is_numeric_v<Delay>)
  void Agent<Delay>::incrementDelay() {
    if (m_delay == std::numeric_limits<Delay>::max()) {
      pConsoleLogger->critical(
          "Agent {} delay has reached its maximum value ({}).", m_id, m_delay);
      std::abort();
    }
    ++m_delay;
  }
  template <typename Delay>
    requires(is_numeric_v<Delay>)
  void Agent<Delay>::incrementDelay(Delay const delay) {
    if (m_delay + delay < m_delay) {
      pConsoleLogger->critical(
          "Agent {} delay has reached its maximum value ({}).", m_id, m_delay);
      std::abort();
    }
    m_delay += delay;
  }
  template <typename Delay>
    requires(is_numeric_v<Delay>)
  void Agent<Delay>::decrementDelay() {
    if (m_delay == 0) {
      throw std::underflow_error(buildLog("Delay has reached its minimum value"));
    }
    --m_delay;
  }

  template <typename Delay>
    requires(is_numeric_v<Delay>)
  void Agent<Delay>::incrementDistance(double distance) {
    if (distance < 0) {
      throw std::invalid_argument(buildLog("Distance travelled must be positive"));
    }
    m_distance += distance;
  }

  template <typename Delay>
    requires(is_numeric_v<Delay>)
  void Agent<Delay>::incrementTime() {
    if (m_time == std::numeric_limits<unsigned int>::max()) {
      pConsoleLogger->critical(
          "Agent {} time has reached its maximum value ({}).", m_id, m_time);
      std::abort();
    }
    ++m_time;
  }
  template <typename Delay>
    requires(is_numeric_v<Delay>)
  void Agent<Delay>::incrementTime(unsigned int const time) {
    if (m_time + time < m_time) {
      pConsoleLogger->critical(
          "Agent {} time has reached its maximum value ({}).", m_id, m_time);
      std::abort();
    }
    m_time += time;
  }
};  // namespace dsm
