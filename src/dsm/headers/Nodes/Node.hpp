/// @file       /src/dsm/headers/Node.hpp
/// @brief      Defines the Node class.
///
/// @details    This file contains the definition of the Node class.
///             The Node class represents a node in the network. It is templated by the type
///             of the node's id, which must be an unsigned integral type.

#ifndef Node_hpp
#define Node_hpp

#include <concepts>
#include <optional>
#include <string>
#include <stdexcept>
#include <vector>
#include <utility>

#include "../../utility/TypeTraits/is_numeric.hpp"
#include "../../utility/queue.hpp"

namespace dsm {
  /// @brief The Node class represents a node in the network.
  /// @tparam Id The type of the node's id. It must be an unsigned integral type.
  template <typename Id, typename Size, typename Priority>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> && dsm::is_numeric_v<Priority>
  class Node {
  private:
    std::pair<double, double> m_coords;
    Id m_id;
    Size m_capacity;
    dsm::priority_queue<Id, Size> m_queue;

  public:
    Node() = delete;
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

    /// @brief Get the node's id
    /// @return Id The node's id
    Id id() const;
    /// @brief Get the node's coordinates
    /// @return std::pair<double,, double> A std::pair containing the node's coordinates
    const std::pair<double, double>& coords() const;
    /// @brief Get the node's capacity
    /// @return Size The node's capacity
    Size capacity() const;
    /// @brief Get the node's queue
    /// @return const dsm::priority_queue<Id, Priority>& The node's queue
    const dsm::priority_queue<Id, Priority>& queue() const;

    /// @brief Returns true if the node is full
    /// @return bool True if the node is full
    virtual bool isFull() const = 0;
    virtual bool isFull(Id streetId) const = 0;
  };

  template <typename Id, typename Size, typename Priority>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  Node<Id, Size, Priority>::Node(Id id) : m_id{id}, m_capacity{std::numeric_limits<Size>::max()} {}
  template <typename Id, typename Size, typename Priority>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  Node<Id, Size, Priority>::Node(Id id, std::pair<double, double> coords)
      : m_coords{coords}, m_id{id}, m_capacity{std::numeric_limits<Size>::max()} {}

  template <typename Id, typename Size, typename Priority>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  void Node<Id, Size, Priority>::setCoords(std::pair<double, double> coords) {
    m_coords = std::move(coords);
  }
  template <typename Id, typename Size, typename Priority>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  void Node<Id, Size, Priority>::setCapacity(Size capacity) {
    m_capacity = capacity;
  }

  template <typename Id, typename Size, typename Priority>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  Id Node<Id, Size, Priority>::id() const {
    return m_id;
  }
  template <typename Id, typename Size, typename Priority>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  const std::pair<double, double>& Node<Id, Size, Priority>::coords() const {
    return m_coords;
  }
  template <typename Id, typename Size, typename Priority>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  Size Node<Id, Size, Priority>::capacity() const {
    return m_capacity;
  }

};  // namespace dsm

#endif
