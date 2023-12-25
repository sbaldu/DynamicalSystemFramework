
#ifndef TrafficLight_hpp
#define TrafficLight_hpp

#include <concepts>
#include <type_traits>
#include <optional>
#include <unordered_map>

#include "Node.hpp"

namespace dsm {

  template <typename Delay>
  struct TrafficLightCycle {
    Delay green;
    Delay red;

    TrafficLightCycle() = delete;
    TrafficLightCycle(Delay green, Delay red) : green{green}, red{red} {}
    TrafficLightCycle(const std::pair<Delay, Delay>& pair)
        : green{pair.first}, red{pair.second} {}
  };

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             std::unsigned_integral<Delay>
  class TrafficLight : public Node<Id, Size> {
  private:
	std::vector<Size> m_passingAgents;
  std::unordered_map<Id, uint8_t> m_streetPriorities;
  std::optional<TrafficLightCycle<Delay>> m_delay;
  Delay m_counter;

  public:
    TrafficLight() = delete;
    /// @brief Construct a new TrafficLight object
    /// @param id The node's id
    explicit TrafficLight(Id id);

    /// @brief Set the node's delay
    /// @param delay The node's delay
    void setDelay(Delay delay);
    /// @brief Set the node's delay
    /// @param delay The node's delay
    void setDelay(const std::pair<Delay, Delay>& delay);
    /// @brief Set the node's phase
    /// @param phase The node's phase
    /// @throw std::runtime_error if the delay is not set
    void setPhase(Delay phase);
    /// @brief Set the node's street priority
    /// @details This function is used to set the node's street priority.
    ///          If priority is positive, the light will follow the TrafficLightCycle green-red.
    ///          If priority is negative, the light will follow the inverse TrafficLightCycle red-green.
    /// @param streetId The id of the street that is connected to the node
    void setStreetPriority(Id streetId, uint8_t priority);
    /// @brief Set the node's street priorities
    /// @details This function is used to set the node's street priorities. If
    ///          priority is positive, the light will follow the TrafficLightCycle green-red.
    ///          If priority is negative, the light will follow the inverse TrafficLightCycle red-green.
    /// @param priorities The node's street priorities
    void setStreetPriorities(const std::unordered_map<Id, uint8_t>& priorities);
    /// @brief Increase the node's counter
    /// @details This function is used to increase the node's counter
    ///          when the simulation is running. It automatically resets the counter
    ///          when it reaches the double of the delay value.
    /// @throw std::runtime_error if the delay is not set
    void increaseCounter();

	/// @brief Insert an agent in the node
	/// @param agentId The id of the agent to insert
    void addAgent(Size agentId);
	/// @brief Remove an agent from the node
	/// @param agentId The id of the agent to remove
    void removeAgent(Size agentId);

	/// @brief Get vector of agents inside the node
	/// @return The reference to the vector of agent ids
    std::vector<Size>& agents();
	/// @brief Get vector of agents inside the node
	/// @return The const reference to the vector of agent ids
    const std::vector<Size>& agents() const;

    /// @brief Get the node's delay
    /// @return std::optional<Delay> The node's delay
    std::optional<Delay> delay() const;
    /// @brief Check if the node's queue is full or the light is red
    /// @return true if the node's queue is full or if the node's counter is less than the first value of the delay
    ///         false otherwise
    /// @throw std::runtime_error if the delay is not set
    bool isFull() const override;
    /// @brief Check if the node's queue is full or the light is red
    /// @details The light is red if the node's counter is less than the first value of the delay for the streets with negative priority
    ///          and if the node's counter is greater than the first value of the delay for the streets with positive priority
    /// @param streetId The id of the street that is connected to the node
    /// @return true if the node's queue is full or if the street has red light
    ///         false otherwise
    /// @throw std::runtime_error if the delay is not set
    bool isFull(Id streetId) const override;
  };

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
                 std::unsigned_integral<Delay>
  TrafficLight<Id, Size, Delay>::TrafficLight(Id id)
      : Node<Id, Size>{id}, m_counter{0} {}

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             std::unsigned_integral<Delay>
  void TrafficLight<Id, Size, Delay>::setDelay(Delay delay) {
    m_delay.green = delay;
    m_delay.red = delay;
  }

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             std::unsigned_integral<Delay>
  void TrafficLight<Id, Size, Delay>::setDelay(
      const std::pair<Delay, Delay>& delay) {
    m_delay.green = delay.first;
    m_delay.red = delay.second;
  }

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             std::unsigned_integral<Delay>
  void TrafficLight<Id, Size, Delay>::setPhase(Delay phase) {
    if (!m_delay.has_value()) {
      std::string errorMsg{"Error at line " + std::to_string(__LINE__) + " in file " +
                           __FILE__ + ": " + "TrafficLight's delay is not set"};
      throw std::runtime_error(errorMsg);
    }
    if (phase > m_delay.value().first + m_delay.value().second) {
      phase -= m_delay.value().first + m_delay.value().second;
    }
    phase == 0 ? m_counter = 0 : m_counter = m_delay.value().first % phase;
  }

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             std::unsigned_integral<Delay>
    void TrafficLight<Id, Size, Delay>::setStreetPriority(Id streetId, uint8_t priority) {
    m_streetPriorities[streetId] = priority;
  }
  
  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
               std::unsigned_integral<Delay>
    void TrafficLight<Id, Size, Delay>::setStreetPriorities(const std::unordered_map<Id, uint8_t>& priorities) {
    m_streetPriorities = std::move(priorities);
  }

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             std::unsigned_integral<Delay>
  void TrafficLight<Id, Size, Delay>::increaseCounter() {
    if (!m_delay.has_value()) {
      std::string errorMsg{"Error at line " + std::to_string(__LINE__) + " in file " +
                           __FILE__ + ": " + "TrafficLight's delay is not set"};
      throw std::runtime_error(errorMsg);
    }
    ++m_counter;
    if (m_counter == m_delay.value().first + m_delay.value().second) {
      m_counter = 0;
    }
  }

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             std::unsigned_integral<Delay>
  void TrafficLight<Id, Size, Delay>::addAgent(Size agentId) {
	m_passingAgents.push_back(agentId);
  }

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             std::unsigned_integral<Delay>
  void TrafficLight<Id, Size, Delay>::removeAgent(Size agentId) {
	std::erase(std::find(m_passingAgents.begin(), m_passingAgents.end(), agentId));
  }

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             std::unsigned_integral<Delay>
  std::vector<Size>& TrafficLight<Id, Size, Delay>::agents() {
	return m_passingAgents;
  }

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             std::unsigned_integral<Delay>
  const std::vector<Size>& TrafficLight<Id, Size, Delay>::agents() const {
	return m_passingAgents;
  }

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             std::unsigned_integral<Delay>
  std::optional<Delay> TrafficLight<Id, Size, Delay>::delay() const {
    return m_delay;
  }

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             std::unsigned_integral<Delay>
  bool TrafficLight<Id, Size, Delay>::isFull() const {
    if (!m_delay.has_value()) {
      std::string errorMsg{"Error at line " + std::to_string(__LINE__) + " in file " +
                           __FILE__ + ": " + "TrafficLight's delay is not set"};
      throw std::runtime_error(errorMsg);
    }
    if (this->m_capacity == m_passingAgents.size()) {
      return true;
    }
    return m_counter < m_delay.value().first;
  }

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             std::unsigned_integral<Delay>
  bool TrafficLight<Id, Size, Delay>::isFull(Id streetId) const {
    if (!m_delay.has_value()) {
      std::string errorMsg{"Error at line " + std::to_string(__LINE__) + " in file " +
                           __FILE__ + ": " + "TrafficLight's delay is not set"};
      throw std::runtime_error(errorMsg);
    }
    if (this->m_capacity == m_passingAgents.size()) {
      return true;
    }
    if (m_counter < m_delay.value().first) {
      return m_streetPriorities[streetId] < 0;
    }
    return m_streetPriorities[streetId] > 0;
  }
};  // namespace dsm

#endif
