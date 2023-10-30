/// @file       src/Node.hpp
/// @brief      Defines the Node class.
///
/// @details    This file contains the definition of the Node class.
///             The Node class represents a node in the network. It is templated by the type
///             of the node's id, which must be an unsigned integral type.

#ifndef Node_hpp
#define Node_hpp

#include <functional>
#include <queue>
#include <utility>

namespace dmf {
  /// @brief The Node class represents a node in the network.
  /// @tparam Id The type of the node's id. It must be an unsigned integral type.
  template <typename Id>
    requires std::unsigned_integral<Id>
  class Node {
  private:
    std::queue<Id> m_queue;
    std::pair<double, double> m_coords;
    Id m_id;

  public:
    Node() = default;
    /// @brief Construct a new Node object
    /// @param id The node's id
    explicit Node(Id id);
    /// @brief Construct a new Node object
    /// @param id The node's id
    /// @param coords A std::pair containing the node's coordinates
    Node(Id id, std::pair<double, double> coords);
    /// @brief Construct a new Node object
    /// @param id The node's id
    /// @param coords A std::pair containing the node's coordinates
    /// @param queue A std::queue containing the node's queue
    Node(Id id, std::pair<double, double> coords, std::queue<Id> queue);

    /// @brief Set the node's coordinates
    /// @param coords A std::pair containing the node's coordinates
    void setCoords(std::pair<double, double> coords);
    /// @brief Set the node's queue
    /// @param queue A std::queue containing the node's queue
    void setQueue(std::queue<Id> queue);

    /// @brief Get the node's id
    /// @return Id The node's id
    Id id() const;
    /// @brief Get the node's coordinates
    /// @return std::pair<double, double> A std::pair containing the node's coordinates
    const std::pair<double, double>& coords() const;
    /// @brief Get the node's queue
    /// @return std::queue<Id> A std::queue containing the node's queue
    const std::queue<Id>& queue() const;
  };

  template <typename Id>
    requires std::unsigned_integral<Id>
  Node<Id>::Node(Id id) : m_id{id} {}

  template <typename Id>
    requires std::unsigned_integral<Id>
  Node<Id>::Node(Id id, std::pair<double, double> coords) : m_coords{std::move(coords)}, m_id{id} {}

  template <typename Id>
    requires std::unsigned_integral<Id>
  Node<Id>::Node(Id id, std::pair<double, double> coords, std::queue<Id> queue)
      : m_queue{std::move(queue)}, m_coords{std::move(coords)}, m_id{id} {}

  template <typename Id>
    requires std::unsigned_integral<Id>
  Id Node<Id>::id() const {
    return m_id;
  }

  template <typename Id>
    requires std::unsigned_integral<Id>
  void Node<Id>::setCoords(std::pair<double, double> coords) {
    m_coords = std::move(coords);
  }

  template <typename Id>
    requires std::unsigned_integral<Id>
  void Node<Id>::setQueue(std::queue<Id> queue) {
    m_queue = std::move(queue);
  }

  template <typename Id>
    requires std::unsigned_integral<Id>
  const std::pair<double, double>& Node<Id>::coords() const {
    return m_coords;
  }

  template <typename Id>
    requires std::unsigned_integral<Id>
  const std::queue<Id>& Node<Id>::queue() const {
    return m_queue;
  }

  // to be implemented
  /* template <typename Id> */
  /* class Intersection : public Node<Id> { */
  /* private: */
  /*   std::function<void()> m_priority; */
  /* }; */

  /* template <typename Id> */
  /* class Roundabout : public Node<Id> { */
  /* private: */
  /*   std::function<void()> m_priority; */
  /* }; */

  /* template <typename Id> */
  /* class TrafficLight : public Node<Id> { */
  /* private: */
  /*   std::function<void()> m_priority; */
  /* }; */
};  // namespace dmf

#endif
