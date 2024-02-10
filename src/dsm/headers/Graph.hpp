/// @file       /src/dsm/headers/Graph.hpp
/// @file       /src/dsm/headers/Graph.hpp
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
#include <optional>
#include <ranges>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <type_traits>
#include <utility>
#include <string>
#include <fstream>
#include <sstream>

#include "Node.hpp"
#include "SparseMatrix.hpp"
#include "Street.hpp"
#include "../utility/DijkstraResult.hpp"
#include "../utility/Logger.hpp"
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
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  class Graph {
  private:
    std::unordered_map<Id, shared<Node<Id, Size>>> m_nodes;
    std::unordered_map<Id, shared<Street<Id, Size>>> m_streets;
    shared<SparseMatrix<Id, bool>> m_adjacency;
    std::unordered_map<Id, Id> m_nodeMapping;

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
    /// @brief Build the graph's street angles using the node's coordinates
    void buildStreetAngles();

    /// @brief Import the graph's adjacency matrix from a file.
    /// If the file is not of a supported format, it will read the file as a matrix with the first two elements being
    /// the number of rows and columns and the following elements being the matrix elements.
    /// @param fileName, The name of the file to import the adjacency matrix from.
    /// @param isAdj A boolean value indicating if the file contains the adjacency matrix or the distance matrix.
    /// @throws std::invalid_argument if the file is not found or invalid
    /// The matrix format is deduced from the file extension. Currently only .dsm files are supported.
    void importMatrix(const std::string& fileName, bool isAdj = true);
    /// @brief Import the graph's nodes from a file
    /// @param fileName The name of the file to import the nodes from.
    /// @throws std::invalid_argument if the file is not found, invalid or the format is not supported
    void importOSMNodes(const std::string& fileName);
    /// @brief Import the graph's streets from a file
    /// @param fileName The name of the file to import the streets from.
    /// @throws std::invalid_argument if the file is not found, invalid or the format is not supported
    void importOSMEdges(const std::string& fileName);

    /// @brief Add a node to the graph
    /// @param node, A std::shared_ptr to the node to add
    void addNode(shared<Node<Id, Size>> node);
    /// @brief Add a node to the graph
    /// @param node, A reference to the node to add
    void addNode(const Node<Id, Size>& node);

    template <typename... Tn>
      requires(is_node_v<std::remove_reference_t<Tn>> && ...)
    void addNodes(Tn&&... nodes);

    template <typename T1, typename... Tn>
      requires is_node_v<std::remove_reference_t<T1>> &&
               (is_node_v<std::remove_reference_t<Tn>> && ...)
    void addNodes(T1&& node, Tn&&... nodes);

    /// @brief Add a street to the graph
    /// @param street, A std::shared_ptr to the street to add
    void addStreet(shared<Street<Id, Size>> street);
    /// @brief Add a street to the graph
    /// @param street, A reference to the street to add
    void addStreet(const Street<Id, Size>& street);

    template <typename T1>
      requires is_street_v<std::remove_reference_t<T1>>
    void addStreets(T1&& street);

    template <typename T1, typename... Tn>
      requires is_street_v<std::remove_reference_t<T1>> &&
               (is_street_v<std::remove_reference_t<Tn>> && ...)
    void addStreets(T1&& street, Tn&&... streets);

    /// @brief Get the graph's adjacency matrix
    /// @return A std::shared_ptr to the graph's adjacency matrix
    shared<SparseMatrix<Id, bool>> adjMatrix() const;
    /// @brief Get the graph's node map
    /// @return A std::unordered_map containing the graph's nodes
    std::unordered_map<Id, shared<Node<Id, Size>>> nodeSet() const;
    /// @brief Get the graph's street map
    /// @return A std::unordered_map containing the graph's streets
    std::unordered_map<Id, shared<Street<Id, Size>>> streetSet() const;
    /// @brief Get a street from the graph
    /// @param source The source node
    /// @param destination The destination node
    /// @return A std::optional containing a std::shared_ptr to the street if it exists, otherwise
    /// std::nullopt
    std::optional<shared<Street<Id, Size>>> street(Id source, Id destination) const;

    /// @brief Get the shortest path between two nodes using dijkstra algorithm
    /// @param source, The source node
    /// @param destination, The destination node
    /// @return A DijkstraResult object containing the path and the distance
    std::optional<DijkstraResult<Id>> shortestPath(
        const Node<Id, Size>& source, const Node<Id, Size>& destination) const;
    /// @brief Get the shortest path between two nodes using dijkstra algorithm
    /// @param source, The source node id
    /// @param destination, The destination node id
    /// @return A DijkstraResult object containing the path and the distance
    std::optional<DijkstraResult<Id>> shortestPath(Id source, Id destination) const;
  };

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  Graph<Id, Size>::Graph() : m_adjacency{make_shared<SparseMatrix<Id, bool>>()} {}

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  Graph<Id, Size>::Graph(const SparseMatrix<Id, bool>& adj)
      : m_adjacency{make_shared<SparseMatrix<Id, bool>>(adj)} {
    std::ranges::for_each(std::views::iota(0, (int)adj.getColDim()), [this](auto i) -> void {
      m_nodes.emplace(std::make_pair(i, make_shared<Node<Id, Size>>(i)));
    });

    std::ranges::for_each(std::views::iota(0, (int)adj.size()), [this, adj](auto i) -> void {
      this->m_streets.emplace(std::make_pair(
          i, make_shared<Street<Id, Size>>(i, std::make_pair(i / adj.getColDim(), i % adj.getColDim()))));
    });
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  Graph<Id, Size>::Graph(const std::unordered_map<Id, shared<Street<Id, Size>>>& streetSet)
      : m_adjacency{make_shared<SparseMatrix<Id, bool>>()} {
    for (const auto& street : streetSet) {
      m_streets.emplace(std::make_pair(street->id(), street));

      Id node1 = street->nodePair().first;
      Id node2 = street->nodePair().second;
      m_nodes.emplace(node1, make_shared<Node<Id, Size>>(node1));
      m_nodes.emplace(node2, make_shared<Node<Id, Size>>(node2));
    }

    buildAdj();
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Graph<Id, Size>::buildAdj() {
    // find max values in streets node pairs
    const size_t maxNode{m_nodes.size()};
    m_adjacency->reshape(maxNode, maxNode);
    for (const auto& street : m_streets) {
      m_adjacency->insert(
          street.second->nodePair().first, street.second->nodePair().second, true);
    }
  }
  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  void Graph<Id, Size>::buildStreetAngles() {
    for (const auto& street : m_streets) {
      const auto& node1{m_nodes[street.second->nodePair().first]};
      const auto& node2{m_nodes[street.second->nodePair().second]};
      street.second->setAngle(node1->coordinates(), node2->coordinates());
    }
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Graph<Id, Size>::importMatrix(const std::string& fileName, bool isAdj) {
    // check the file extension
    std::string fileExt = fileName.substr(fileName.find_last_of(".") + 1);
    if (fileExt == "dsm") {
      std::ifstream file{fileName};
      if (!file.is_open()) {
        throw std::invalid_argument(buildLog("Cannot find file: " + fileName));
      }
      Id rows, cols;
      file >> rows >> cols;
      if (rows != cols) {
        throw std::invalid_argument(buildLog("Adjacency matrix must be square"));
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
        m_nodes.emplace(node1, make_shared<Node<Id, Size>>(node1));
        m_nodes.emplace(node2, make_shared<Node<Id, Size>>(node2));
        m_streets.emplace(index, make_shared<Street<Id, Size>>(index, std::make_pair(node1, node2)));
        if (!isAdj) {
          m_streets[index]->setLength(val);
        }
      }
    } else {
      // default case: read the file as a matrix with the first two elements being the number of rows and columns and
      // the following elements being the matrix elements
      std::ifstream file{fileName};
      if (!file.is_open()) {
        throw std::invalid_argument(buildLog("Cannot find file: " + fileName));
      }
      Id rows, cols;
      file >> rows >> cols;
      if (rows != cols) {
        throw std::invalid_argument(buildLog("Adjacency matrix must be square"));
      }
      m_adjacency = make_shared<SparseMatrix<Id, bool>>(rows, cols);
      Id index{0};
      while (!file.eof()) {
        double value;
        file >> value;
        if (value < 0) {
          throw std::invalid_argument(
              buildLog("Adjacency matrix elements must be positive"));
        }
        if (value > 0) {
          m_adjacency->insert(index, true);
          const Id node1{static_cast<Id>(index / rows)};
          const Id node2{static_cast<Id>(index % cols)};
          m_nodes.emplace(node1, make_shared<Node<Id, Size>>(node1));
          m_nodes.emplace(node2, make_shared<Node<Id, Size>>(node2));
          m_streets.emplace(index,
                                     make_shared<Street<Id, Size>>(index, std::make_pair(node1, node2)));
          if (!isAdj) {
            m_streets[index]->setLength(value);
          }
        }
        ++index;
      }
    }
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Graph<Id, Size>::importOSMNodes(const std::string& fileName) {
    std::string fileExt = fileName.substr(fileName.find_last_of(".") + 1);
    if (fileExt == "csv") {
      std::ifstream file{fileName};
      if (!file.is_open()) {
        throw std::invalid_argument(buildLog("Cannot find file: " + fileName));
      }
      std::string line;
      std::getline(file, line);  // skip first line
      Id nodeIndex{0};
      while (!file.eof()) {
        std::getline(file, line);
        if (line.empty()) {
          continue;
        }
        std::istringstream iss{line};
        std::string id, lat, lon, highway;
        // osmid;x;y;highway
        std::getline(iss, id, ';');
        std::getline(iss, lat, ';');
        std::getline(iss, lon, ';');
        std::getline(iss, highway, ';');
        Id nodeId{static_cast<Id>(std::stoul(id))};
        m_nodes.emplace(
            nodeIndex,
            make_shared<Node<Id, Size>>(nodeIndex,
                                        std::make_pair(std::stod(lat), std::stod(lon))));
        m_nodeMapping.emplace(std::make_pair(nodeId, nodeIndex));
        ++nodeIndex;
      }
    } else {
      throw std::invalid_argument(buildLog("File extension not supported"));
    }
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Graph<Id, Size>::importOSMEdges(const std::string& fileName) {
    std::string fileExt = fileName.substr(fileName.find_last_of(".") + 1);
    if (fileExt == "csv") {
      std::ifstream file{fileName};
      if (!file.is_open()) {
        std::string errrorMsg{"Error at line " + std::to_string(__LINE__) + " in file " +
                              __FILE__ + ": " + "File not found"};
        throw std::invalid_argument(errrorMsg);
      }
      std::string line;
      std::getline(file, line);  // skip first line
      while (!file.eof()) {
        std::getline(file, line);
        if (line.empty()) {
          continue;
        }
        std::istringstream iss{line};
        std::string sourceId, targetId, length, oneway, highway, maxspeed, bridge;
        // u;v;length;oneway;highway;maxspeed;bridge
        std::getline(iss, sourceId, ';');
        std::getline(iss, targetId, ';');
        std::getline(iss, length, ';');
        std::getline(iss, oneway, ';');
        std::getline(iss, highway, ';');
        std::getline(iss, maxspeed, ';');
        std::getline(iss, bridge, ';');
        try {
          std::stod(maxspeed);
        } catch (const std::invalid_argument& e) {
          maxspeed = "30";
        }
        Id streetId = std::stoul(sourceId) + std::stoul(targetId) * m_nodes.size();
        m_streets.emplace(
            streetId,
            make_shared<Street<Id, Size>>(
                streetId,
                1,
                std::stod(maxspeed),
                std::stod(length),
                std::make_pair(m_nodeMapping[std::stoul(sourceId)],
                               m_nodeMapping[std::stoul(targetId)])));
      }
    } else {
      std::string errrorMsg{"Error at line " + std::to_string(__LINE__) + " in file " +
                            __FILE__ + ": " + "File extension not supported"};
      throw std::invalid_argument(errrorMsg);
    }
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Graph<Id, Size>::addNode(shared<Node<Id, Size>> node) {
    m_nodes.emplace(std::make_pair(node->id(), node));
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Graph<Id, Size>::addNode(const Node<Id, Size>& node) {
    m_nodes.emplace(std::make_pair(node.id(), make_shared<Node<Id, Size>>(node)));
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  template <typename... Tn>
    requires(is_node_v<std::remove_reference_t<Tn>> && ...)
  void Graph<Id, Size>::addNodes(Tn&&... nodes) {}

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  template <typename T1, typename... Tn>
    requires is_node_v<std::remove_reference_t<T1>> &&
             (is_node_v<std::remove_reference_t<Tn>> && ...)
  void Graph<Id, Size>::addNodes(T1&& node, Tn&&... nodes) {
    addNode(std::forward<T1>(node));
    addNodes(std::forward<Tn>(nodes)...);
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Graph<Id, Size>::addStreet(shared<Street<Id, Size>> street) {
    // emplace street
    m_streets.emplace(std::make_pair(street->id(), street));
    // emplace nodes
    const Id node1{street.nodePair().first};
    const Id node2{street.nodePair().second};
    m_nodes.emplace(node1);
    m_nodes.emplace(node2);
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Graph<Id, Size>::addStreet(const Street<Id, Size>& street) {
    // emplace street
    m_streets.emplace(std::make_pair(street.id(), make_shared<Street<Id, Size>>(street)));
    // emplace nodes
    const Id node1{street.nodePair().first};
    const Id node2{street.nodePair().second};
    m_nodes.emplace(node1, make_shared<Node<Id, Size>>(node1));
    m_nodes.emplace(node2, make_shared<Node<Id, Size>>(node2));
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  template <typename T1>
    requires is_street_v<std::remove_reference_t<T1>>
  void Graph<Id, Size>::addStreets(T1&& street) {
    // emplace street
    m_streets.emplace(std::make_pair(street.id(), make_shared<Street<Id, Size>>(street)));
    // emplace nodes
    const Id node1{street.nodePair().first};
    const Id node2{street.nodePair().second};
    m_nodes.emplace(node1, make_shared<Node<Id, Size>>(node1));
    m_nodes.emplace(node2, make_shared<Node<Id, Size>>(node2));
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  template <typename T1, typename... Tn>
    requires is_street_v<std::remove_reference_t<T1>> &&
             (is_street_v<std::remove_reference_t<Tn>> && ...)
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
  std::unordered_map<Id, shared<Node<Id, Size>>> Graph<Id, Size>::nodeSet() const {
    return m_nodes;
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  std::unordered_map<Id, shared<Street<Id, Size>>> Graph<Id, Size>::streetSet() const {
    return m_streets;
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  std::optional<shared<Street<Id, Size>>> Graph<Id, Size>::street(Id source, Id destination) const {
    auto streetIt =
        std::find_if(m_streets.begin(), m_streets.end(), [source, destination](const auto& street) -> bool {
          return street.second->nodePair().first == source &&
                 street.second->nodePair().second == destination;
        });
    if (streetIt == m_streets.end()) {
      return std::nullopt;
    }
    return streetIt->second;
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  std::optional<DijkstraResult<Id>> Graph<Id, Size>::shortestPath(const Node<Id, Size>& source,
                                                                  const Node<Id, Size>& destination) const {
    return this->shortestPath(source.id(), destination.id());
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  std::optional<DijkstraResult<Id>> Graph<Id, Size>::shortestPath(Id source, Id destination) const {
	const Id sourceId{source};

    std::unordered_map<Id, shared<Node<Id, Size>>> unvisitedNodes{m_nodes};
    if (!unvisitedNodes.contains(source)) {
      return std::nullopt;
    }
    if (!unvisitedNodes.contains(destination)) {
      return std::nullopt;
    }

    const size_t n_nodes{m_nodes.size()};
    auto adj{*m_adjacency};

    std::unordered_set<Id> visitedNodes;
    std::vector<std::pair<Id, double>> dist(n_nodes);
    std::for_each(dist.begin(), dist.end(), [count = 0](auto& element) mutable -> void {
      element.first = count;
      element.second = std::numeric_limits<double>::max();
      ++count;
    });
    dist[source] = std::make_pair(source, 0.);

    std::vector<std::pair<Id, double>> prev(n_nodes);
	std::for_each(prev.begin(), prev.end(), [](auto& pair) -> void {
		  pair.first = std::numeric_limits<Id>::max();
		  pair.second = std::numeric_limits<double>::max();
		});
    prev[source].second = 0.;

    while (unvisitedNodes.size() != 0) {
      source = std::min_element(unvisitedNodes.begin(),
                                unvisitedNodes.end(),
                                [&dist](const auto& a, const auto& b) -> bool {
                                  return dist[a.first].second < dist[b.first].second;
                                })
                   ->first;
      unvisitedNodes.erase(source);
      visitedNodes.emplace(source);

      const auto& neighbors{adj.getRow(source)};
      for (const auto& neighbour : neighbors) {
        // if the node has already been visited, skip it
        if (visitedNodes.find(neighbour.first) != visitedNodes.end()) {
          continue;
        }
        
        double streetLength = this->street(source, neighbour.first).value()->length();
        // if current path is shorter than the previous one, update the distance
        if (streetLength + dist[source].second < dist[neighbour.first].second) {
          dist[neighbour.first].second = streetLength + dist[source].second;
          prev[neighbour.first] = std::make_pair(source, dist[neighbour.first].second);
        }
      }

      adj.emptyColumn(source);
    }

    std::vector<Id> path{destination};
    Id previous{destination};
    while (true) {
      previous = prev[previous].first;
      if (previous == std::numeric_limits<Id>::max()) {
        return std::nullopt;
      }
      path.push_back(previous);
	  if (previous == sourceId) {
		break;
	  }
    }

    std::reverse(path.begin(), path.end());
    return DijkstraResult<Id>(path, prev[destination].second);
  }
};  // namespace dsm

#endif