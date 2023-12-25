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
  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  class Node {
  protected:
    std::pair<double, double> m_coords;
    Id m_id;
    Size m_capacity;
    Size m_transportCapacity;

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
    /// @brief Set the node's transport capacity
    /// @details The transport capacity is the maximum number of agents that can traverse the node
    ///          in a time step.
    /// @param capacity The node's transport capacity
    void setTransportCapacity(Size capacity);

    /// @brief Get the node's id
    /// @return Id The node's id
    Id id() const;
    /// @brief Get the node's coordinates
    /// @return std::pair<double,, double> A std::pair containing the node's coordinates
    const std::pair<double, double>& coords() const;
    /// @brief Get the node's capacity
    /// @return Size The node's capacity
    Size capacity() const;
    /// @brief Get the node's transport capacity
    /// @details The transport capacity is the maximum number of agents that can traverse the node
    ///          in a time step.
    /// @return Size The node's transport capacity
    Size transportCapacity() const;

    /// @brief Returns true if the node is full
    /// @return bool True if the node is full
    virtual bool isFull() const = 0;
    /// @brief Returns true if the node is full as seen from the street with the given id
    /// @param streetId The id of the street
    /// @return bool True if the node is full
    virtual bool isFull(Id streetId) const = 0;
  };

  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  Node<Id, Size>::Node(Id id) : m_id{id}, m_capacity{1}, m_transportCapacity{std::numeric_limits<Size>::max()} {}
  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  Node<Id, Size>::Node(Id id, std::pair<double, double> coords)
      : m_coords{coords}, m_id{id}, m_capacity{1}, m_transportCapacity{std::numeric_limits<Size>::max()} {}

  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  void Node<Id, Size>::setCoords(std::pair<double, double> coords) {
    m_coords = std::move(coords);
  }
  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  void Node<Id, Size>::setCapacity(Size capacity) {
    m_capacity = capacity;
  }
  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  void Node<Id, Size>::setTransportCapacity(Size capacity) {
    m_transportCapacity = capacity;
  }

  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  Id Node<Id, Size>::id() const {
    return m_id;
  }
  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  const std::pair<double, double>& Node<Id, Size>::coords() const {
    return m_coords;
  }
  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  Size Node<Id, Size>::capacity() const {
    return m_capacity;
  }
  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  Size Node<Id, Size>::transportCapacity() const {
    return m_transportCapacity;
  }

};  // namespace dsm

#endif
