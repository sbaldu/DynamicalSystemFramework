/// @file       src/Graph.hpp
/// @brief      Defines the Graph class.
///
/// @details    This file contains the definition of the Graph class.
///             The Graph class represents a graph in the network. It is templated by the type
///             of the graph's id and the type of the graph's capacity.
///             The graph's id and capacity must be unsigned integral types.

#ifndef Graph_hpp
#define Graph_hpp

#include <algorithm>
#include <concepts>
#include <limits>
#include <memory>
#include <ranges>
#include <unordered_set>
#include <queue>
#include <type_traits>
#include <utility>

#include "Node.hpp"
#include "SparseMatrix.hpp"
#include "Street.hpp"
#include "utility/HashFunctions.hpp"
#include "utility/TypeTraits/is_node.hpp"
#include "utility/TypeTraits/is_street.hpp"

namespace dmf {

  // Alias for shared pointers
  template <typename T>
  using shared = std::shared_ptr<T>;
  using std::make_shared;

  /// @brief The Graph class represents a graph in the network.
  /// @tparam Id, The type of the graph's id. It must be an unsigned integral type.
  /// @tparam Size, The type of the graph's capacity. It must be an unsigned integral type.
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  class Graph {
  private:
    std::unordered_set<shared<Node<Id>>, nodeHash<Id>> m_nodes;
    std::unordered_set<shared<Street<Id, Size>>, streetHash<Id, Size>> m_streets;
    shared<SparseMatrix<Id, bool>> m_adjacency;

  public:
    Graph();
    /// @brief Construct a new Graph object
    /// @param adj, An adjacency matrix made by a SparseMatrix representing the graph's adjacency matrix
    Graph(const SparseMatrix<Id, bool>& adj);
    /// @brief Construct a new Graph object
    /// @param streetSet, A set of streets representing the graph's streets
    Graph(const std::unordered_set<shared<Street<Id, Size>>, nodeHash<Id>>& streetSet);

    /// @brief Build the graph's adjacency matrix
    void buildAdj();

    /// @brief Add a node to the graph
    /// @param node, A std::shared_ptr to the node to add
    void addNode(shared<Node<Id>> node);
    /// @brief Add a node to the graph
    /// @param node, A reference to the node to add
    void addNode(const Node<Id>& node);

    template <typename... Tn>
      requires(is_node_v<Tn> && ...)
    void addNodes(Tn&&... nodes);

    template <typename T1, typename... Tn>
      requires is_node_v<T1> && (is_node_v<Tn> && ...)
    void addNodes(T1&& node, Tn&&... nodes);

    /// @brief Add a street to the graph
    /// @param street, A std::shared_ptr to the street to add
    void addStreet(shared<Street<Id, Size>> street);
    /// @brief Add a street to the graph
    /// @param street, A reference to the street to add
    void addStreet(const Street<Id, Size>& street);

    template <typename... Tn>
      requires(is_street_v<Tn> && ...)
    void addStreets(Tn&&... streets);

    template <typename T1, typename... Tn>
      requires is_street_v<T1> && (is_street_v<Tn> && ...)
    void addStreets(T1&& street, Tn&&... streets);

    /// @brief Get the graph's adjacency matrix
    /// @return A std::shared_ptr to the graph's adjacency matrix
    shared<SparseMatrix<Id, bool>> adjMatrix() const;
    /// @brief Get the graph's node set
    /// @return A std::unordered_set containing the graph's nodes
    std::unordered_set<shared<Node<Id>>, nodeHash<Id>> nodeSet() const;
    /// @brief Get the graph's street set
    /// @return A std::unordered_set containing the graph's streets
    std::unordered_set<shared<Street<Id, Size>>, streetHash<Id, Size>> streetSet() const;
  };

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  Graph<Id, Size>::Graph() : m_adjacency{make_shared<SparseMatrix<Id, bool>>()} {}

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  Graph<Id, Size>::Graph(const SparseMatrix<Id, bool>& adj)
      : m_adjacency{make_shared<SparseMatrix<Id, bool>>(adj)} {
    std::ranges::for_each(std::views::iota(0, (int)adj.getColDim()),
                          [this](auto i) -> void { m_nodes.insert(make_shared<Node<Id>>(i)); });

    std::ranges::for_each(std::views::iota(0, (int)adj.size()), [this](auto i) -> void {
      this->m_streets.insert(make_shared<Street<Id, Size>>(i));
    });
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  Graph<Id, Size>::Graph(const std::unordered_set<shared<Street<Id, Size>>, nodeHash<Id>>& streetSet)
      : m_adjacency{make_shared<SparseMatrix<Id, bool>>()} {
    for (auto street : streetSet) {
      m_streets.insert(street);
      m_nodes.insert(street->nodes().first);
      m_nodes.insert(street->nodes().second);
    }

    buildAdj();
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Graph<Id, Size>::buildAdj() {
    // find max values in streets node pairs
    Id maxNode = 0;
    for (auto street : m_streets) {
      if (street->nodePair().first > maxNode) {
        maxNode = street->nodePair().first;
      }
      if (street->nodePair().second > maxNode) {
        maxNode = street->nodePair().second;
      }
    }
    m_adjacency->reshape(maxNode + 1);
    for (auto street : m_streets) {
      m_adjacency->insert(street->nodePair().first, street->nodePair().second, true);
      m_adjacency->insert(street->nodePair().second, street->nodePair().first, true);
    }
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Graph<Id, Size>::addNode(shared<Node<Id>> node) {
    m_nodes.insert(node);
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Graph<Id, Size>::addNode(const Node<Id>& node) {
    m_nodes.insert(make_shared<Node<Id>>(node));
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  template <typename... Tn>
    requires(is_node_v<Tn> && ...)
  void Graph<Id, Size>::addNodes(Tn&&... nodes) {}

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  template <typename T1, typename... Tn>
    requires is_node_v<T1> && (is_node_v<Tn> && ...)
  void Graph<Id, Size>::addNodes(T1&& node, Tn&&... nodes) {
    addNode(std::forward<T1>(node));
    addNodes(std::forward<Tn>(nodes)...);
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Graph<Id, Size>::addStreet(shared<Street<Id, Size>> street) {
    m_streets.insert(street);
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Graph<Id, Size>::addStreet(const Street<Id, Size>& street) {
    m_streets.insert(make_shared<Street<Id, Size>>(street));
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  template <typename... Tn>
    requires(is_street_v<Tn> && ...)
  void Graph<Id, Size>::addStreets(Tn&&... edges) {}

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  template <typename T1, typename... Tn>
    requires is_street_v<T1> && (is_street_v<Tn> && ...)
  void Graph<Id, Size>::addStreets(T1&& street, Tn&&... streets) {
    addStreet(std::forward<T1>(street));
    addStreets(std::forward<Tn>(streets)...);
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  shared<SparseMatrix<Id, bool>> Graph<Id, Size>::adjMatrix() const {
    return m_adjacency;
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  std::unordered_set<shared<Node<Id>>, nodeHash<Id>> Graph<Id, Size>::nodeSet() const {
    return m_nodes;
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  std::unordered_set<shared<Street<Id, Size>>, streetHash<Id, Size>> Graph<Id, Size>::streetSet() const {
    return m_streets;
  }
};  // namespace dmf

#endif
