/// @file       /src/dsm/headers/Node.hpp
/// @brief      Defines the Node class.
///
/// @details    This file contains the definition of the Node class.
///             The Node class represents a node in the network. It is templated by the type
///             of the node's id, which must be an unsigned integral type.
///             The derived classes are:
///             - TrafficLight: represents a traffic light node

#ifndef Node_hpp
#define Node_hpp

#include <functional>
#include <utility>
#include <stdexcept>
#include <optional>
#include <set>
#include <map>

#include "../utility/Logger.hpp"

namespace dsm {
  /// @brief The Node class represents a node in the network.
  /// @tparam Id The type of the node's id. It must be an unsigned integral type.
  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  class Node {
  protected:
    std::multimap<int16_t, Id> m_agents;
    /* I don't actually know if it is better yo use a std::map or a priority_queue...
    Using the second one means that the node is blocked if an agent with priority cannot move.
    The first is just like an ordering...
    Need to discuss this.*/
    std::set<Id> m_streetPriorities; // A set containing the street ids that have priority - like main roads
    std::pair<double, double> m_coords;
    Id m_id;
    Size m_capacity;
    Size m_agentCounter;

  public:
    Node() = default;
    /// @brief Construct a new Node object
    /// @param id The node's id
    explicit Node(Id id);
    /// @brief Construct a new Node object
    /// @param id The node's id
    /// @param coords A std::pair containing the node's coordinates
    Node(Id id, std::pair<double, double> coords);

    /// @brief Set the node's coordinates
    /// @param coords A std::pair containing the node's coordinates
    void setCoords(std::pair<double, double> coords);
    /// @brief Set the node's capacity
    /// @param capacity The node's capacity
    void setCapacity(Size capacity);
    /// @brief Put an agent in the node
    /// @param agent A std::pair containing the agent's angle difference and id
    /// @details The agent's angle difference is used to order the agents in the node.
    ///          The agent with the smallest angle difference is the first one to be
    ///          removed from the node.
    /// @throws std::runtime_error if the node is full
    void addAgent(std::pair<double, Id> agent);
    /// @brief Put an agent in the node
    /// @param agentId The agent's id
    /// @details The agent's angle difference is used to order the agents in the node.
    ///          The agent with the smallest angle difference is the first one to be
    ///          removed from the node.
    /// @throws std::runtime_error if the node is full
    void addAgent(Id agentId);
    /// @brief Removes an agent from the node
    /// @param agentId The agent's id
    void removeAgent(Id agentId);
    /// @brief Set the node streets with priority
    /// @param streetPriorities A std::set containing the node's street priorities
    void setStreetPriorities(std::set<Id> streetPriorities);
    /// @brief Add a street to the node street priorities
    /// @param streetId The street's id
    void addStreetPriority(Id streetId);

    virtual bool isGreen() const;
    virtual bool isGreen(Id) const;
    virtual void increaseCounter() {};

    /// @brief Get the node's id
    /// @return Id The node's id
    /// @return Id The node's id
    Id id() const;
    /// @brief Get the node's coordinates
    /// @return std::pair<double,, double> A std::pair containing the node's coordinates
    const std::pair<double, double>& coords() const;
    /// @brief Get the node's street priorities
    /// @details This function returns a std::set containing the node's street priorities.
    ///        If a street has priority, it means that the agents that are on that street
    ///        have priority over the agents that are on the other streets.
    /// @return std::set<Id> A std::set containing the node's street priorities
    virtual const std::set<Id>& streetPriorities() const;
    /// @brief Get the node's capacity
    /// @return Size The node's capacity
    Size capacity() const;
    /// @brief Get the node's agent ids
    /// @return std::set<Id> A std::set containing the node's agent ids
    std::multimap<int16_t, Id> agents() const;
    /// @brief Returns true if the node is full
    /// @return bool True if the node is full
    bool isFull() const;
    /// @brief Returns the number of agents that have passed through the node
    /// @return Size The number of agents that have passed through the node
    /// @details This function returns the number of agents that have passed through the node
    ///          since the last time this function was called. It also resets the counter.
    Size agentCounter();
  };

  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  Node<Id, Size>::Node(Id id) : m_id{id}, m_capacity{1}, m_agentCounter{0} {}

  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  Node<Id, Size>::Node(Id id, std::pair<double, double> coords)
      : m_coords{std::move(coords)}, m_id{id}, m_capacity{1}, m_agentCounter{0} {}
  
  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  bool Node<Id, Size>::isGreen() const {
    throw std::runtime_error(buildLog("isGreen() is not implemented for this type of node."));
  }
  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  bool Node<Id, Size>::isGreen(Id) const {
    throw std::runtime_error(buildLog("isGreen() is not implemented for this type of node."));
  }

  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  Id Node<Id, Size>::id() const {
    return m_id;
  }

  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  void Node<Id, Size>::setCoords(std::pair<double, double> coords) {
    m_coords = std::move(coords);
  }

  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  void Node<Id, Size>::setStreetPriorities(std::set<Id> streetPriorities) {
    m_streetPriorities = std::move(streetPriorities);
  }

  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  void Node<Id, Size>::addStreetPriority(Id streetId) {
    m_streetPriorities.emplace(streetId);
  }

  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  void Node<Id, Size>::setCapacity(Size capacity) {
    if (capacity < m_agents.size()) {
      throw std::runtime_error(
          buildLog("Node capacity is smaller than the current queue size"));
    }
    m_capacity = capacity;
  }

  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  void Node<Id, Size>::addAgent(Id agentId) {
    if (m_agents.size() == m_capacity) {
      throw std::runtime_error(buildLog("Node is full"));
    }
    for (auto const& agent : m_agents) {
      if (agent.second == agentId) {
        throw std::runtime_error(buildLog("Agent is already on the node"));
      }
    }
    int lastKey{0};
    if (!m_agents.empty()) {
      lastKey = m_agents.rbegin()->first + 1;
    }
    m_agents.emplace(lastKey, agentId);
    ++m_agentCounter;
  }

  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  void Node<Id, Size>::removeAgent(Id agentId) {
    for (auto it{m_agents.begin()}; it != m_agents.end(); ++it) {
      if (it->second == agentId) {
        m_agents.erase(it);
        return;
      }
    }
    throw std::runtime_error(buildLog("Agent is not on the node"));
  }

  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  const std::pair<double, double>& Node<Id, Size>::coords() const {
    return m_coords;
  }

  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  const std::set<Id>& Node<Id, Size>::streetPriorities() const {
    return m_streetPriorities;
  }

  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  Size Node<Id, Size>::capacity() const {
    return m_capacity;
  }

  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  std::multimap<int16_t, Id> Node<Id, Size>::agents() const {
    return m_agents;
  }

  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  bool Node<Id, Size>::isFull() const {
    return m_agents.size() == m_capacity;
  }

  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  Size Node<Id, Size>::agentCounter() {
    Size copy{m_agentCounter};
    m_agentCounter = 0;
    return copy;
  }

  // to be implemented
  /* template <typename Id> */
  /* class Intersection : public Node<Id, Size> { */
  /* private: */
  /*   std::function<void()> m_priority; */
  /* }; */

  /* template <typename Id> */
  /* class Roundabout : public Node<Id, Size> { */
  /* private: */
  /*   std::function<void()> m_priority; */
  /* }; */

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>
  class TrafficLight : public Node<Id, Size> {
  private:
    std::optional<std::pair<Delay, Delay>> m_delay;
    Delay m_counter;

  public:
    TrafficLight() = default;
    /// @brief Construct a new TrafficLight object
    /// @param id The node's id
    TrafficLight(Id id);

    /// @brief Set the node's delay
    /// @param delay The node's delay
    void setDelay(Delay delay);
    /// @brief Set the node's delay
    /// @param delay The node's delay
    void setDelay(std::pair<Delay, Delay> delay);
    /// @brief Set the node's phase
    /// @param phase The node's phase
    /// @throw std::runtime_error if the delay is not set
    void setPhase(Delay phase);
    /// @brief Increase the node's counter
    /// @details This function is used to increase the node's counter
    ///          when the simulation is running. It automatically resets the counter
    ///          when it reaches the double of the delay value.
    /// @throw std::runtime_error if the delay is not set
    void increaseCounter() override;

    /// @brief Get the node's delay
    /// @return std::optional<Delay> The node's delay
    std::optional<Delay> delay() const;
    Delay counter() const { return m_counter; }
    /// @brief Returns true if the traffic light is green
    /// @return bool True if the traffic light is green
    bool isGreen() const override;
    bool isGreen(Id streetId) const override;
  };

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>
  TrafficLight<Id, Size, Delay>::TrafficLight(Id id) : Node<Id, Size>{id}, m_counter{0} {}

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>
  void TrafficLight<Id, Size, Delay>::setDelay(Delay delay) {
    m_delay = std::make_pair(delay, delay);
  }
  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>
  void TrafficLight<Id, Size, Delay>::setDelay(std::pair<Delay, Delay> delay) {
    m_delay = std::move(delay);
  }
  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>
  void TrafficLight<Id, Size, Delay>::setPhase(Delay phase) {
    if (!m_delay.has_value()) {
      throw std::runtime_error(buildLog("TrafficLight's delay has not been set."));
    }
    if (phase > m_delay.value().first + m_delay.value().second) {
      phase -= m_delay.value().first + m_delay.value().second;
    }
    phase == 0 ? m_counter = 0 : m_counter = m_delay.value().first % phase; //fwefbewbfw
  }
  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>
  void TrafficLight<Id, Size, Delay>::increaseCounter() {
    if (!m_delay.has_value()) {
      throw std::runtime_error(buildLog("TrafficLight's delay has not been set."));
    }
    ++m_counter;
    if (m_counter == m_delay.value().first + m_delay.value().second) {
      m_counter = 0;
    }
  }

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>
  std::optional<Delay> TrafficLight<Id, Size, Delay>::delay() const {
    return m_delay;
  }
  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>
  bool TrafficLight<Id, Size, Delay>::isGreen() const {
    if (!m_delay.has_value()) {
      throw std::runtime_error(buildLog("TrafficLight's delay has not been set."));
    }
    return m_counter < m_delay.value().first;
  }
  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>
  bool TrafficLight<Id, Size, Delay>::isGreen(Id streetId) const {
    if (!m_delay.has_value()) {
      throw std::runtime_error(buildLog("TrafficLight's delay has not been set."));
    }
    bool hasPriority{this->streetPriorities().contains(streetId)};
    if (this->isGreen()) {
      return hasPriority;
    }
    return !hasPriority;
  }
};  // namespace dsm

#endif
