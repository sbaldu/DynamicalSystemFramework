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
#include <unordered_map>
#include <queue>
#include <type_traits>
#include <utility>
#include <string>
#include <fstream>

#include "Node.hpp"
#include "SparseMatrix.hpp"
#include "Street.hpp"
#include "../utility/TypeTraits/is_node.hpp"
#include "../utility/TypeTraits/is_street.hpp"

namespace dsm {

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
    std::unordered_map<Id, shared<Node<Id>>> m_nodes;
    std::unordered_map<Id, shared<Street<Id, Size>>> m_streets;
    shared<SparseMatrix<Id, bool>> m_adjacency;

  public:
    Graph();
    /// @brief Construct a new Graph object
    /// @param adj, An adjacency matrix made by a SparseMatrix representing the graph's adjacency matrix
    Graph(const SparseMatrix<Id, bool>& adj);
    /// @brief Construct a new Graph object
    /// @param streetSet, A map of streets representing the graph's streets
    Graph(const std::unordered_map<Id, shared<Street<Id, Size>>>& streetSet);

    /// @brief Build the graph's adjacency matrix
    void buildAdj();

    /// @brief Import the graph's adjacency matrix from a file
    /// @param fileName, The name of the file to import the adjacency matrix from.
    /// @throws std::invalid_argument if the file is not found or the format is not supported
    /// The matrix format is deduced from the file extension. Currently only .dsm files are supported.
    void importAdj(const std::string& fileName);

    /// @brief Add a node to the graph
    /// @param node, A std::shared_ptr to the node to add
    void addNode(shared<Node<Id>> node);
    /// @brief Add a node to the graph
    /// @param node, A reference to the node to add
    void addNode(const Node<Id>& node);

    template <typename... Tn>
      requires(is_node_v<std::remove_reference_t<Tn>> && ...)
    void addNodes(Tn&&... nodes);

    template <typename T1, typename... Tn>
      requires is_node_v<std::remove_reference_t<T1>> && (is_node_v<std::remove_reference_t<Tn>> && ...)
    void addNodes(T1&& node, Tn&&... nodes);

    /// @brief Add a street to the graph
    /// @param street, A std::shared_ptr to the street to add
    void addStreet(shared<Street<Id, Size>> street);
    /// @brief Add a street to the graph
    /// @param street, A reference to the street to add
    void addStreet(const Street<Id, Size>& street);

    template <typename T1>
      requires is_street_v<std::remove_reference_t<T1>>
    void addStreets(T1&& streets);

    template <typename T1, typename... Tn>
      requires is_street_v<std::remove_reference_t<T1>> && (is_street_v<std::remove_reference_t<Tn>> && ...)
    void addStreets(T1&& street, Tn&&... streets);

    /// @brief Get the graph's adjacency matrix
    /// @return A std::shared_ptr to the graph's adjacency matrix
    shared<SparseMatrix<Id, bool>> adjMatrix() const;
    /// @brief Get the graph's node map
    /// @return A std::unordered_map containing the graph's nodes
    std::unordered_map<Id, shared<Node<Id>>> nodeSet() const;
    /// @brief Get the graph's street map
    /// @return A std::unordered_map containing the graph's streets
    std::unordered_map<Id, shared<Street<Id, Size>>> streetSet() const;
  };

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  Graph<Id, Size>::Graph() : m_adjacency{make_shared<SparseMatrix<Id, bool>>()} {}

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  Graph<Id, Size>::Graph(const SparseMatrix<Id, bool>& adj)
      : m_adjacency{make_shared<SparseMatrix<Id, bool>>(adj)} {
    std::ranges::for_each(std::views::iota(0, (int)adj.getColDim()), [this](auto i) -> void {
      m_nodes.insert(std::make_pair(i, make_shared<Node<Id>>(i)));
    });

    std::ranges::for_each(std::views::iota(0, (int)adj.size()), [this, adj](auto i) -> void {
      this->m_streets.insert(std::make_pair(
          i, make_shared<Street<Id, Size>>(i, std::make_pair(i / adj.getColDim(), i % adj.getColDim()))));
    });
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  Graph<Id, Size>::Graph(const std::unordered_map<Id, shared<Street<Id, Size>>>& streetSet)
      : m_adjacency{make_shared<SparseMatrix<Id, bool>>()} {
    for (const auto& street : streetSet) {
      m_streets.insert(std::make_pair(street->id(), street));

      Id node1 = street->nodePair().first;
      Id node2 = street->nodePair().second;
      m_nodes.insert(node1, make_shared<Node<Id>>(node1));
      m_nodes.insert(node2, make_shared<Node<Id>>(node2));
    }

    buildAdj();
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Graph<Id, Size>::buildAdj() {
    // find max values in streets node pairs
    const size_t maxNode{m_nodes.size()};
    m_adjacency->reshape(maxNode);
    for (const auto& street : m_streets) {
      m_adjacency->insert(street.second->nodePair().first, street.second->nodePair().second, true);
    }
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Graph<Id, Size>::importAdj(const std::string& fileName) {
    // check the file extension
    std::string fileExt = fileName.substr(fileName.find_last_of(".") + 1);
    if (fileExt == "dsm") {
      std::ifstream file(fileName);
      if (!file.is_open()) {
        std::string errrorMsg =
            "Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " + "File not found";
        throw std::invalid_argument(errrorMsg);
      }
      Id rows, cols;
      file >> rows >> cols;
      if (rows != cols) {
        std::string errrorMsg = "Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ +
                                ": " + "Adjacency matrix must be square";
        throw std::invalid_argument(errrorMsg);
      }
      m_adjacency = make_shared<SparseMatrix<Id, bool>>(rows, cols);
      // each line has (should have) 3 elements
      while (!file.eof()) {
        Id index;
        bool val;
        file >> index >> val;
        m_adjacency->insert(index, val);
        const Id node1{static_cast<Id>(index / rows)};
        const Id node2{static_cast<Id>(index % cols)};
        m_nodes.insert_or_assign(node1, make_shared<Node<Id>>(node1));
        m_nodes.insert_or_assign(node2, make_shared<Node<Id>>(node2));
        m_streets.insert_or_assign(index,
                                   make_shared<Street<Id, Size>>(index, std::make_pair(node1, node2)));
      }
    } else {
      std::string errrorMsg = "Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
                              "File extension not supported";
      throw std::invalid_argument(errrorMsg);
    }
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Graph<Id, Size>::addNode(shared<Node<Id>> node) {
    m_nodes.insert(std::make_pair(node->id(), node));
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Graph<Id, Size>::addNode(const Node<Id>& node) {
    m_nodes.insert(std::make_pair(node.id(), make_shared<Node<Id>>(node)));
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  template <typename... Tn>
    requires(is_node_v<std::remove_reference_t<Tn>> && ...)
  void Graph<Id, Size>::addNodes(Tn&&... nodes) {}

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  template <typename T1, typename... Tn>
    requires is_node_v<std::remove_reference_t<T1>> && (is_node_v<std::remove_reference_t<Tn>> && ...)
  void Graph<Id, Size>::addNodes(T1&& node, Tn&&... nodes) {
    addNode(std::forward<T1>(node));
    addNodes(std::forward<Tn>(nodes)...);
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Graph<Id, Size>::addStreet(shared<Street<Id, Size>> street) {
    // insert street
    m_streets.insert(std::make_pair(street->id(), street));
    // insert nodes
    const Id node1{street.nodePair().first};
    const Id node2{street.nodePair().second};
    m_nodes.insert_or_assign(node1);
    m_nodes.insert_or_assign(node2);
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Graph<Id, Size>::addStreet(const Street<Id, Size>& street) {
    // insert street
    m_streets.insert(std::make_pair(street.id(), make_shared<Street<Id, Size>>(street)));
    // insert nodes
    const Id node1{street.nodePair().first};
    const Id node2{street.nodePair().second};
    m_nodes.insert_or_assign(node1, make_shared<Node<Id>>(node1));
    m_nodes.insert_or_assign(node2, make_shared<Node<Id>>(node2));
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  template <typename T1>
    requires is_street_v<std::remove_reference_t<T1>>
  void Graph<Id, Size>::addStreets(T1&& street) {
    // insert street
    m_streets.insert(std::make_pair(street.id(), make_shared<Street<Id, Size>>(street)));
    // insert nodes
    const Id node1{street.nodePair().first};
    const Id node2{street.nodePair().second};
    m_nodes.insert_or_assign(node1, make_shared<Node<Id>>(node1));
    m_nodes.insert_or_assign(node2, make_shared<Node<Id>>(node2));
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  template <typename T1, typename... Tn>
    requires is_street_v<std::remove_reference_t<T1>> && (is_street_v<std::remove_reference_t<Tn>> && ...)
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
  std::unordered_map<Id, shared<Node<Id>>> Graph<Id, Size>::nodeSet() const {
    return m_nodes;
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  std::unordered_map<Id, shared<Street<Id, Size>>> Graph<Id, Size>::streetSet() const {
    return m_streets;
  }
};  // namespace dsm

#endif
