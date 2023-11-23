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

namespace dsm {
  /// @brief The Street class represents a street in the network.
  /// @tparam Id, The type of the street's id. It must be an unsigned integral type.
  /// @tparam Size, The type of the street's capacity. It must be an unsigned integral type.
  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  class Street {
  private:
    std::queue<Size> m_queue;
    std::pair<Id, Id> m_nodePair;
    double m_len;
    double m_maxSpeed;
    Id m_id;
    Size m_size;
    Size m_capacity;

  public:
    Street() = delete;
    /// @brief Construct a new Street object
    /// @param index, The street's id
    /// @param nodePair, The street's node pair
    Street(Id index, std::pair<Id, Id> nodePair);
    /// @brief Construct a new Street object
    /// @param index, The street's id
    /// @param capacity, The street's capacity
    /// @param len, The street's length
    /// @param nodePair, The street's node pair
    Street(Id index, Size capacity, double len, std::pair<Id, Id> nodePair);
    /// @brief Construct a new Street object
    /// @param index, The street's id
    /// @param capacity, The street's capacity
    /// @param len, The street's length
    /// @param maxSpeed, The street's speed limit
    /// @param nodePair, The street's node pair
    Street(Id index, Size capacity, double len, double maxSpeed, std::pair<Id, Id> nodePair);

    /// @brief Set the street's id
    /// @param id, The street's id
    void setId(Id id);
    /// @brief Set the street's capacity
    /// @param capacity, The street's capacity
    void setCapacity(Size capacity);
    /// @brief Set the street's length
    /// @param len, The street's length
    void setLength(double len);
    /// @brief Set the street's queue
    /// @param queue, The street's queue
    void setQueue(std::queue<Size> queue);
    /// @brief Set the street's node pair
    /// @param node1, The source node of the street
    /// @param node2, The destination node of the street
    void setNodePair(Id node1, Id node2);
    /// @brief Set the street's node pair
    /// @param node1, The source node of the street
    /// @param node2, The destination node of the street
    void setNodePair(const Node<Id>& node1, const Node<Id>& node2);
    /// @brief Set the street's node pair
    /// @param pair, The street's node pair
    void setNodePair(std::pair<Id, Id> pair);
    /// @brief Set the street's speed limit
    /// @param speed, The street's speed limit
    /// @throw std::invalid_argument, If the speed is negative
    void setMaxSpeed(double speed);

    /// @brief Get the street's id
    /// @return Id, The street's id
    Id id() const;
    /// @brief Get the street's size
    /// @return Size, The street's size
    Size size() const;
    /// @brief Get the street's capacity
    /// @return Size, The street's capacity
    Size capacity() const;
    /// @brief Get the street's length
    /// @return double, The street's length
    double length() const;
    /// @brief Get the street's queue
    /// @return std::queue<Size>, The street's queue
    const std::queue<Size>& queue() const;
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
    /// @param agent, The agent to add
    template <typename Delay>
      requires is_numeric_v<Delay>
    void enqueue(const Agent<Id, Size, Delay>& agent);
    /// @brief Remove an agent from the street's queue
    std::optional<Id> dequeue();
  };

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  Street<Id, Size>::Street(Id index, std::pair<Id, Id> pair)
      : m_nodePair{std::move(pair)}, m_maxSpeed{30.}, m_id{index}, m_size{0} {}

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  Street<Id, Size>::Street(Id index, Size capacity, double len, std::pair<Id, Id> nodePair)
      : m_nodePair{std::move(nodePair)},
        m_len{len},
        m_maxSpeed{30.},
        m_id{index},
        m_size{0},
        m_capacity{capacity} {}

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  Street<Id, Size>::Street(Id index, Size capacity, double len, double maxSpeed, std::pair<Id, Id> nodePair)
      : m_nodePair{std::move(nodePair)}, m_len{len}, m_id{index}, m_size{0}, m_capacity{capacity} {
    this->setMaxSpeed(maxSpeed);
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Street<Id, Size>::setId(Id id) {
    m_id = id;
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Street<Id, Size>::setCapacity(Size capacity) {
    m_capacity = capacity;
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Street<Id, Size>::setLength(double len) {
    m_len = len;
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Street<Id, Size>::setQueue(std::queue<Size> queue) {
    m_queue = std::move(queue);
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Street<Id, Size>::setNodePair(Id node1, Id node2) {
    m_nodePair = std::make_pair(node1, node2);
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Street<Id, Size>::setNodePair(const Node<Id>& node1, const Node<Id>& node2) {
    m_nodePair = std::make_pair(node1.id(), node2.id());
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Street<Id, Size>::setNodePair(std::pair<Id, Id> pair) {
    m_nodePair = std::move(pair);
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Street<Id, Size>::setMaxSpeed(double speed) {
    if (speed < 0.) {
      std::string errorMsg = "Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
                             "The maximum speed of a street cannot be negative.";
      throw std::invalid_argument(errorMsg);
    }
    m_maxSpeed = speed;
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  Id Street<Id, Size>::id() const {
    return m_id;
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  Size Street<Id, Size>::size() const {
    return m_size;
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  Size Street<Id, Size>::capacity() const {
    return m_capacity;
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  double Street<Id, Size>::length() const {
    return m_len;
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  const std::queue<Size>& Street<Id, Size>::queue() const {
    return m_queue;
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  const std::pair<Id, Id>& Street<Id, Size>::nodePair() const {
    return m_nodePair;
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  double Street<Id, Size>::density() const {
    return static_cast<double>(m_size) / m_capacity;
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  double Street<Id, Size>::maxSpeed() const {
    return m_maxSpeed;
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  template <typename Delay>
    requires is_numeric_v<Delay>
  void Street<Id, Size>::enqueue(const Agent<Id, Size, Delay>& agent) {
    if (m_size < m_capacity) {
      m_queue.push(agent.index());
      ++m_size;
    }
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  std::optional<Id> Street<Id, Size>::dequeue() {
    if (m_size > 0) {
      Id res{m_queue.front()};
      m_queue.pop();
      --m_size;

      return res;
    }

    return std::nullopt;
  }

};  // namespace dsm

#endif
