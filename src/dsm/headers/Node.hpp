/// @file       /src/dsm/headers/Node.hpp
/// @brief      Defines the Node class.
///
/// @details    This file contains the definition of the Node class.
///             The Node class represents a node in the network. It is templated by the type
///             of the node's id, which must be an unsigned integral type.

#ifndef Node_hpp
#define Node_hpp

#include <functional>
#include <utility>
#include <string>
#include <stdexcept>
#include <optional>
#include <vector>
#include <map>

namespace dsm {
  /// @brief The Node class represents a node in the network.
  /// @tparam Id The type of the node's id. It must be an unsigned integral type.
  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  class Node {
  protected:
    std::vector<Id> m_agentIds;
    std::map<int16_t, Id> m_streetPriorities;
    std::pair<double, double> m_coords;
    Id m_id;
    Size m_capacity;

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
    /// @brief Puts an agent in the node
    /// @param agentId The agent's id
    void addAgent(Id agentId);
    /// @brief Removes an agent from the node
    /// @param agentId The agent's id
    void removeAgent(Id agentId);
    
    void setStreetPriorities(std::map<int16_t, Id> streetPriorities);

    void addStreetPriority(int16_t priority, Id streetId);

    virtual bool isGreen() const { return false; };
    virtual void increaseCounter() {};

    /// @brief Get the node's id
    /// @return Id The node's id
    Id id() const;
    /// @brief Get the node's coordinates
    /// @return std::pair<double,, double> A std::pair containing the node's coordinates
    const std::pair<double, double>& coords() const;
    /// @brief Get the node's street priorities
    /// @return std::map<Id, Size> A std::map containing the node's street priorities
    /// @details The keys of the map are intended as priorities, while the values are the ids of the streets.
    ///          The streets are ordered by priority, from the highest to the lowest. You should have both positive
    ///          and negative priorities, where the positive ones are the ones that have the green light, and the
    ///          negative ones are the ones that have the red light (and viceversa)
    virtual const std::map<int16_t, Id>& streetPriorities() const;
    /// @brief Get the node's capacity
    /// @return Size The node's capacity
    Size capacity() const;
    /// @brief Get the node's agent ids
    /// @return std::vector<Id> A std::vector containing the node's agent ids
    std::vector<Id> agentIds() const;
    /// @brief Returns true if the node is full
    /// @return bool True if the node is full
    bool isFull() const;
  };

  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  Node<Id, Size>::Node(Id id) : m_id{id}, m_capacity{1} {}

  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  Node<Id, Size>::Node(Id id, std::pair<double, double> coords)
      : m_coords{std::move(coords)}, m_id{id}, m_capacity{1} {}

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
  void Node<Id, Size>::setStreetPriorities(std::map<int16_t, Id> streetPriorities) {
    m_streetPriorities = std::move(streetPriorities);
  }

  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  void Node<Id, Size>::addStreetPriority(int16_t priority, Id streetId) {
    m_streetPriorities.emplace(priority, streetId);
  }

  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  void Node<Id, Size>::setCapacity(Size capacity) {
    if (capacity < m_agentIds.size()) {
      std::string errorMsg{"Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
                           "Node's capacity is smaller than the current size"};
      throw std::invalid_argument(errorMsg);
    }
    m_capacity = capacity;
  }

  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  void Node<Id, Size>::addAgent(Id agentId) {
    if (m_agentIds.size() == m_capacity) {
      std::string errorMsg{"Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
                           "Node is full"};
      throw std::runtime_error(errorMsg);
    }
    m_agentIds.push_back(agentId);
  }

  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  void Node<Id, Size>::removeAgent(Id agentId) {
    auto it = std::find(m_agentIds.begin(), m_agentIds.end(), agentId);
    if (it == m_agentIds.end()) {
      std::string errorMsg{"Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
                           "Agent is not on the node"};
      throw std::runtime_error(errorMsg);
    }
    m_agentIds.erase(it);
  }

  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  const std::pair<double, double>& Node<Id, Size>::coords() const {
    return m_coords;
  }

  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  const std::map<int16_t, Id>& Node<Id, Size>::streetPriorities() const {
    return m_streetPriorities;
  }

  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  Size Node<Id, Size>::capacity() const {
    return m_capacity;
  }

  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  std::vector<Id> Node<Id, Size>::agentIds() const {
    return m_agentIds;
  }

  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  bool Node<Id, Size>::isFull() const {
    return m_agentIds.size() == m_capacity;
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
    std::optional<Delay> m_delay;
    Delay m_counter;

  public:
    TrafficLight() = default;
    /// @brief Construct a new TrafficLight object
    /// @param id The node's id
    TrafficLight(Id id);

    /// @brief Set the node's delay
    /// @param delay The node's delay
    void setDelay(Delay delay);
    /// @brief Set the node's phase
    /// @param phase The node's phase
    /// @throw std::runtime_error if the delay is not set
    void setPhase(Delay phase);
    /// @brief Increase the node's counter
    /// @details This function is used to increase the node's counter
    ///          when the simulation is running. It automatically resets the counter
    ///          when it reaches the double of the delay value.
    /// @throw std::runtime_error if the delay is not set
    void increaseCounter();

    /// @brief Get the node's delay
    /// @return std::optional<Delay> The node's delay
    std::optional<Delay> delay() const;
    Delay counter() const { return m_counter; }
    /// @brief Returns true if the traffic light is green
    /// @return bool True if the traffic light is green
    bool isGreen() const;
  };

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>
  TrafficLight<Id, Size, Delay>::TrafficLight(Id id) : Node<Id, Size>{id}, m_counter{0} {}

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>
  void TrafficLight<Id, Size, Delay>::setDelay(Delay delay) {
    m_delay = delay;
  }
  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>
  void TrafficLight<Id, Size, Delay>::setPhase(Delay phase) {
    if (!m_delay.has_value()) {
      std::string errorMsg{"Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
                           "TrafficLight's delay is not set"};
      throw std::runtime_error(errorMsg);
    }
    phase == 0 ? m_counter = 0 : m_counter = m_delay.value() % phase;
  }
  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>
  void TrafficLight<Id, Size, Delay>::increaseCounter() {
    if (!m_delay.has_value()) {
      std::string errorMsg{"Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
                           "TrafficLight's delay is not set"};
      throw std::runtime_error(errorMsg);
    }
    ++m_counter;
    if (m_counter == 2 * m_delay.value()) {
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
    return m_counter < m_delay;
  }

};  // namespace dsm

#endif
