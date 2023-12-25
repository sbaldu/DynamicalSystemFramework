/// @file       /src/dsm/headers/Street.hpp
/// @brief      Defines the Street class.
///
/// @details    This file contains the definition of the Street class.
///             The Street class represents a street in the network. It is templated by the
///             type of the street's id and the type of the street's capacity.
///             The street's id and capacity must be unsigned integral types.

#ifndef Street_hpp
#define Street_hpp

#include <optional>
#include <queue>
#include <type_traits>
#include <utility>
#include <stdexcept>
#include <string>

#include "Agent.hpp"
#include "Node.hpp"
#include "../utility/TypeTraits/is_numeric.hpp"
#include "../utility/queue.hpp"

namespace dsm {
  /// @brief The Street class represents a street in the network.
  /// @tparam Id, The type of the street's id. It must be an unsigned integral type.
  /// @tparam Size, The type of the street's capacity. It must be an unsigned integral type.
  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  class Street {
  private:
    dsm::queue<Size> m_queue;
    std::pair<Id, Id> m_nodePair;
    double m_len;
    double m_maxSpeed;
    Id m_id;
    Size m_capacity;
    Size m_transportCapacity;

  public:
    Street() = delete;
    /// @brief Construct a new Street object
    /// @param id The street's id
    /// @param nodePair The street's node pair
    Street(Id id, std::pair<Id, Id> nodePair);
    /// @brief Construct a new Street object
    /// @param id The street's id
    /// @param capacity The street's capacity
    /// @param len, The street's length
    /// @param nodePair, The street's node pair
    Street(Id id, Size capacity, double len, std::pair<Id, Id> nodePair);
    /// @brief Construct a new Street object
    /// @param id The street's id
    /// @param capacity The street's capacity
    /// @param len The street's length
    /// @param maxSpeed The street's speed limit
    /// @param nodePair The street's node pair
    Street(Id id, Size capacity, double len, double maxSpeed, std::pair<Id, Id> nodePair);

    /// @brief Set the street's id
    /// @param id The street's id
    void setId(Id id);
    /// @brief Set the street's capacity
    /// @param capacity The street's capacity
    void setCapacity(Size capacity);
    /// @brief Set the street's transport capacity
    /// @details The transport capacity is the maximum number of agents that can traverse the street
    ///          in a time step.
    /// @param capacity The street's transport capacity
    void setTransportCapacity(Size capacity);
    /// @brief Set the street's length
    /// @param len The street's length
    /// @throw std::invalid_argument, If the length is negative
    void setLength(double len);
    /// @brief Set the street's queue
    /// @param queue, The street's queue
    void setQueue(dsm::queue<Size> queue);
    /// @brief Set the street's node pair
    /// @param node1 The source node of the street
    /// @param node2 The destination node of the street
    void setNodePair(Id node1, Id node2);
    /// @brief Set the street's node pair
    /// @param node1 The source node of the street
    /// @param node2 The destination node of the street
    void setNodePair(const Node<Id, Size>& node1, const Node<Id, Size>& node2);
    /// @brief Set the street's node pair
    /// @param pair The street's node pair
    void setNodePair(std::pair<Id, Id> pair);
    /// @brief Set the street's speed limit
    /// @param speed The street's speed limit
    /// @throw std::invalid_argument, If the speed is negative
    void setMaxSpeed(double speed);

    /// @brief Get the street's id
    /// @return Id, The street's id
    Id id() const;
    /// @brief Get the street's capacity
    /// @return Size, The street's capacity
    Size capacity() const;
    /// @brief Get the street's transport capacity
    /// @details The transport capacity is the maximum number of agents that can traverse the street
    ///          in a time step.
    /// @return Size, The street's transport capacity
    Size transportCapacity() const;
    /// @brief Get the street's length
    /// @return double, The street's length
    double length() const;
    /// @brief Get the street's queue
    /// @return dsm::queue<Size>, The street's queue
    const dsm::queue<Size>& queue() const;
    /// @brief Get the street's node pair
    /// @return std::pair<Id, Id>, The street's node pair
    const std::pair<Id, Id>& nodePair() const;
    /// @brief Get the street's density
    /// @return double, The street's density
    double density() const;
    /// @brief Get the street's speed limit
    /// @return double, The street's speed limit
    double maxSpeed() const;
    /// @brief Add an agent to the street's queue
    /// @param agentId The id of the agent to add to the street's queue
    void enqueue(Id agentId);
    /// @brief Remove an agent from the street's queue
    std::optional<Id> dequeue();
  };

  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  Street<Id, Size>::Street(Id index, std::pair<Id, Id> pair)
      : m_nodePair{std::move(pair)}, m_maxSpeed{13.8888888889}, m_id{index}, m_capacity{1}, m_transportCapacity{std::numeric_limits<Size>::max()} {}

  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  Street<Id, Size>::Street(Id id, Size capacity, double len, std::pair<Id, Id> nodePair)
      : m_nodePair{std::move(nodePair)},
        m_len{len},
        m_maxSpeed{30.},
        m_id{id},
        m_capacity{capacity},
        m_transportCapacity{std::numeric_limits<Size>::max()} {}

  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  Street<Id, Size>::Street(Id id, Size capacity, double len, double maxSpeed, std::pair<Id, Id> nodePair)
      : m_nodePair{std::move(nodePair)}, m_len{len}, m_id{id}, m_capacity{capacity} {
    this->setMaxSpeed(maxSpeed);
  }

  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  void Street<Id, Size>::setId(Id id) {
    m_id = id;
  }
  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  void Street<Id, Size>::setCapacity(Size capacity) {
    m_capacity = capacity;
  }
  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  void Street<Id, Size>::setTransportCapacity(Size capacity) {
    m_transportCapacity = capacity;
  }
  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  void Street<Id, Size>::setLength(double len) {
    if (len < 0.) {
      std::string errorMsg{"Error at line " + std::to_string(__LINE__) + " in file " +
                           __FILE__ + ": " +
                           "The length of a street cannot be negative."};
      throw std::invalid_argument(errorMsg);
    }
    m_len = len;
  }
  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  void Street<Id, Size>::setQueue(dsm::queue<Size> queue) {
    m_queue = std::move(queue);
  }
  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  void Street<Id, Size>::setNodePair(Id node1, Id node2) {
    m_nodePair = std::make_pair(node1, node2);
  }
  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  void Street<Id, Size>::setNodePair(const Node<Id, Size>& node1,
                                     const Node<Id, Size>& node2) {
    m_nodePair = std::make_pair(node1.id(), node2.id());
  }
  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  void Street<Id, Size>::setNodePair(std::pair<Id, Id> pair) {
    m_nodePair = std::move(pair);
  }
  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  void Street<Id, Size>::setMaxSpeed(double speed) {
    if (speed < 0.) {
      std::string errorMsg{"Error at line " + std::to_string(__LINE__) + " in file " +
                           __FILE__ + ": " +
                           "The maximum speed of a street cannot be negative."};
      throw std::invalid_argument(errorMsg);
    }
    m_maxSpeed = speed;
  }

  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  Id Street<Id, Size>::id() const {
    return m_id;
  }
  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  Size Street<Id, Size>::capacity() const {
    return m_capacity;
  }
  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  Size Street<Id, Size>::transportCapacity() const {
    return m_transportCapacity;
  }
  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  double Street<Id, Size>::length() const {
    return m_len;
  }
  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  const dsm::queue<Size>& Street<Id, Size>::queue() const {
    return m_queue;
  }
  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  const std::pair<Id, Id>& Street<Id, Size>::nodePair() const {
    return m_nodePair;
  }
  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  double Street<Id, Size>::density() const {
    return static_cast<double>(m_queue.size()) / m_capacity;
  }
  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  double Street<Id, Size>::maxSpeed() const {
    return m_maxSpeed;
  }

  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  void Street<Id, Size>::enqueue(Id agentId) {
    if (m_queue.size() == m_capacity) {
      std::string errorMsg{"Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
                           "The street's queue is full."};
      throw std::runtime_error(errorMsg);
    }
    m_queue.push(agentId);
  }
  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  std::optional<Id> Street<Id, Size>::dequeue() {
    if(m_queue.empty()) {
      return std::nullopt;
    }
    Id id = m_queue.front();
    m_queue.pop();
    return id;
  }

};  // namespace dsm

#endif
