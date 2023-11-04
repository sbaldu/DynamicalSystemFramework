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

    template <typename T1>
      requires is_street_v<T1>
    void addStreets(T1&& street);

    template <typename T1, typename... Tn>
      requires is_street_v<T1> && (is_street_v<Tn> && ...)
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
    requires is_street_v<T1>
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
  std::unordered_map<Id, shared<Node<Id>>> Graph<Id, Size>::nodeSet() const {
    return m_nodes;
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  std::unordered_map<Id, shared<Street<Id, Size>>> Graph<Id, Size>::streetSet() const {
    return m_streets;
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  std::optional<dt> Graph<Id, Size>::dijkstra(const Node<Id>& source, const Node<Id>& dest) const {
    dt result;

    auto source_node_it = std::find_if(nodeSet.begin(), nodeSet.end(), [&source](auto node) {
      return node->getUserId() == source.getUserId();
    });
    if (source_node_it == nodeSet.end()) {
      // check if source node exist in the graph
      return std::nullopt;
    }

    auto target_node_it = std::find_if(nodeSet.begin(), nodeSet.end(), [&target](auto node) {
      return node->getUserId() == target.getUserId();
    });
    if (target_node_it == nodeSet.end()) {
      // check if target node exist in the graph
      return std::nullopt;
    }
    // n denotes the number of vertices in graph
    auto n = m_nodes.size();

    // setting all the distances initially to INF_DOUBLE
    std::unordered_map<shared<const Node<Id>>, double, nodeHash<Id>> dist;

    for (const auto& node : m_nodes) {
      dist[node] = INF_DOUBLE;
    }

    // creating a min heap using priority queue
    // first element of pair contains the distance
    // second element of pair contains the vertex
    std::priority_queue<std::pair<double, shared<const Node<Id>>>,
                        std::vector<std::pair<double, shared<const Node<Id>>>>,
                        std::greater<std::pair<double, shared<const Node<Id>>>>>
        pq;

    // pushing the source vertex 's' with 0 distance in min heap
    pq.push(std::make_pair(0.0, *source_node_it));

    // marking the distance of source as 0
    dist[*source_node_it] = 0;

    std::unordered_map<std::string, std::string> parent;
    parent[source.getUserId()] = "";

    while (!pq.empty()) {
      // second element of pair denotes the node / vertex
      shared<const Node<Id>> currentNode = pq.top().second;
      // first element of pair denotes the distance
      double currentDist = pq.top().first;

      pq.pop();

      // for all the reachable vertex from the currently exploring vertex
      // we will try to minimize the distance
      if (cachedAdjMatrix->find(currentNode) != cachedAdjMatrix->end()) {
        for (const auto& elem : cachedAdjMatrix->at(currentNode)) {
          // minimizing distances
          if (elem.second->isWeighted().has_value() && elem.second->isWeighted().value()) {
            if (elem.second->isDirected().has_value() && elem.second->isDirected().value()) {
              shared<const DirectedWeightedEdge> dw_edge =
                  std::static_pointer_cast<const DirectedWeightedEdge>(elem.second);
              if (dw_edge->getWeight() < 0) {
                result.errorMessage = ERR_NEGATIVE_WEIGHTED_EDGE;
                return result;
              } else if (currentDist + dw_edge->getWeight() < dist[elem.first]) {
                dist[elem.first] = currentDist + dw_edge->getWeight();
                pq.push(std::make_pair(dist[elem.first], elem.first));
                parent[elem.first.get()->getUserId()] = currentNode.get()->getUserId();
              }
            } else if (elem.second->isDirected().has_value() && !elem.second->isDirected().value()) {
              shared<const UndirectedWeightedEdge> udw_edge =
                  std::static_pointer_cast<const UndirectedWeightedEdge>(elem.second);
              if (udw_edge->getWeight() < 0) {
                result.errorMessage = ERR_NEGATIVE_WEIGHTED_EDGE;
                return result;
              } else if (currentDist + udw_edge->getWeight() < dist[elem.first]) {
                dist[elem.first] = currentDist + udw_edge->getWeight();
                pq.push(std::make_pair(dist[elem.first], elem.first));
                parent[elem.first.get()->getUserId()] = currentNode.get()->getUserId();
              }
            } else {
              // ERROR it shouldn't never returned ( does not exist a Node
              // Weighted and not Directed/Undirected)
              result.errorMessage = ERR_NO_DIR_OR_UNDIR_EDGE;
              return result;
            }
          } else {
            // No Weighted Edge
            result.errorMessage = ERR_NO_WEIGHTED_EDGE;
            return result;
          }
        }
      }
    }
    if (dist[*target_node_it] != INF_DOUBLE) {
      result.success = true;
      result.errorMessage = "";
      result.result = dist[*target_node_it];
      std::string id = target.getUserId();
      while (parent[id] != "") {
        result.path.push_back(id);
        id = parent[id];
      }
      result.path.push_back(source.getUserId());
      std::reverse(result.path.begin(), result.path.end());
      return result;
    }
    result.errorMessage = ERR_TARGET_NODE_NOT_REACHABLE;
    return result;
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  std::optional<dt> Graph<Id, Size>::dijkstra(const Node<Id>& source, const Node<Id>& dest) const {
    std::vector<int> dist;  // The output array. dist.at(i) will hold the shortest
    // distance from src to i
    dist.reserve(_n);

    std::vector<bool> sptSet;  // sptSet.at(i) will be true if vertex i is included in shortest
    // path tree or shortest distance from src to i is finalized
    sptSet.reserve(_n);

    // Initialize all distances as INFINITE and stpSet as false
    for (int i{}; i < _n; ++i) {
      dist.push_back(std::numeric_limits<int>::max());
	  sptSet.push_back(false);
	}

    // Distance of source vertex from itself is always 0
    dist[source.id()] = 0;

    // Find shortest path for all vertices
    for (int count{}; count < _n - 1; ++count) {
      // Pick the minimum distance vertex from the set of vertices not
      // yet processed. u is always equal to src in the first iteration.
      int u{minDistance(dist, sptSet, _n)};

      // Mark the picked vertex as processed
      sptSet[u] = true;

      // Update dist value of the adjacent vertices of the picked vertex.
      for (int v{}; v < _n; ++v) {
        // Update dist.at(v) only if is not in sptSet, there is an edge from
        // u to v, and total weight of path from src to v through u is
        // smaller than current value of dist.at(v)

        // auto length = _adjMatrix.at(u).at(v);
        int time{};
        if (_adjMatrix.contains(u, v)) {
          auto weight = _streets[_findStreet(u, v)]->getLength();
          weight /= this->_getStreetMeanVelocity(_findStreet(u, v));
          time = static_cast<int>(weight);
        }
        if (!sptSet[v] && time && dist[u] != std::numeric_limits<int>::max() && dist[u] + time < dist[v])
          dist[v] = dist[u] + time;
      }
    }

    return dist[dst];
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  std::optional<dt> Graph<Id, Size>::shortestPath(Id source, Id dest) const {
	const Id n_nodes{m_nodes.size()};
    std::vector<int> dist(n_nodes, std::numeric_limits<Id>::max());  // The output array. dist.at(i) will hold the shortest
    // distance from src to i

    std::vector<bool> sptSet(n_nodes, std::numeric_limits<Id>::max());  // sptSet.at(i) will be true if vertex i is included in shortest
    // path tree or shortest distance from src to i is finalized

    // Initialize all distances as INFINITE and stpSet as false
    for (int i{}; i < n_nodes; ++i)
      dist.push_back(std::numeric_limits<int>::max()), sptSet.push_back(false);

    // Distance of source vertex from itself is always 0
    dist[source] = 0;

    // Find shortest path for all vertices
    for (int count{}; count < n_nodes - 1; ++count) {
      // Pick the minimum distance vertex from the set of vertices not
      // yet processed. u is always equal to src in the first iteration.
      int u = minDistance(dist, sptSet, _n);

      // Mark the picked vertex as processed
      sptSet[u] = true;

      // Update dist value of the adjacent vertices of the picked vertex.
      for (int v{}; v < n_nodes; ++v) {
        // Update dist.at(v) only if is not in sptSet, there is an edge from
        // u to v, and total weight of path from src to v through u is
        // smaller than current value of dist.at(v)

        // auto length = _adjMatrix.at(u).at(v);
        int time{};
        if (_adjMatrix.contains(u, v)) {
          auto weight = _streets[_findStreet(u, v)]->getLength();
          weight /= this->_getStreetMeanVelocity(_findStreet(u, v));
          time = static_cast<int>(weight);
        }
        if (!sptSet[v] && time && dist[u] != std::numeric_limits<Id>::max() && dist[u] + time < dist[v])
          dist[v] = dist[u] + time;
      }
    }
    return dist[dst];
  }

};  // namespace dsm

#endif
