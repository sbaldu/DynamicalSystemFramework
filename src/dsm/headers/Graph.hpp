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
#include <cassert>

#include "Node.hpp"
#include "SparseMatrix.hpp"
#include "Street.hpp"
#include "../utility/DijkstraResult.hpp"
#include "../utility/Logger.hpp"
#include "../utility/TypeTraits/is_node.hpp"
#include "../utility/TypeTraits/is_street.hpp"

namespace dsm {

  /// @brief The Graph class represents a graph in the network.
  /// @tparam Id, The type of the graph's id. It must be an unsigned integral type.
  /// @tparam Size, The type of the graph's capacity. It must be an unsigned integral type.
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  class Graph {
  private:
    std::unordered_map<Id, std::unique_ptr<Node<Id, Size>>> m_nodes;
    std::unordered_map<Id, std::unique_ptr<Street<Id, Size>>> m_streets;
    std::unordered_map<Id, Id> m_nodeMapping;
    SparseMatrix<Id, bool> m_adjacency;

    /// @brief Reassign the street ids using the max node id
    /// @details The street ids are reassigned using the max node id, i.e.
    /// newStreetId = srcId * n + dstId, where n is the max node id.
    void m_reassignIds();

  public:
    Graph();
    /// @brief Construct a new Graph object
    /// @param adj An adjacency matrix made by a SparseMatrix representing the graph's adjacency matrix
    Graph(const SparseMatrix<Id, bool>& adj);
    /// @brief Construct a new Graph object
    /// @param streetSet A map of streets representing the graph's streets
    Graph(const std::unordered_map<Id, std::unique_ptr<Street<Id, Size>>>& streetSet);

    Graph(const Graph<Id, Size>& other) {
      std::for_each(other.m_nodes.begin(), other.m_nodes.end(), [this](const auto& pair) {
        this->m_nodes.emplace(pair.first, std::make_unique<Node<Id, Size>>(*pair.second));
      });
      std::for_each(
          other.m_streets.begin(), other.m_streets.end(), [this](const auto& pair) {
            this->m_streets.emplace(pair.first,
                                    std::make_unique<Street<Id, Size>>(*pair.second));
          });
      m_nodeMapping = other.m_nodeMapping;
      m_adjacency = other.m_adjacency;
    }

    Graph& operator=(const Graph<Id, Size>& other) {
      std::for_each(other.m_nodes.begin(), other.m_nodes.end(), [this](const auto& pair) {
        this->m_nodes.insert_or_assign(pair.first,
                                       std::make_unique<Node<Id, Size>>(*pair.second));
      });
      std::for_each(
          other.m_streets.begin(), other.m_streets.end(), [this](const auto& pair) {
            this->m_streets.insert_or_assign(
                pair.first, std::make_unique<Street<Id, Size>>(*pair.second));
          });
      m_nodeMapping = other.m_nodeMapping;
      m_adjacency = other.m_adjacency;

      return *this;
    }

    Graph(Graph<Id, Size>&&) = default;
    Graph& operator=(Graph<Id, Size>&&) = default;

    /// @brief Build the graph's adjacency matrix
    /// @details The adjacency matrix is built using the graph's streets and nodes. N.B.: The street ids
    /// are reassigned using the max node id, i.e. newStreetId = srcId * n + dstId, where n is the max node id.
    void buildAdj();
    /// @brief Build the graph's street angles using the node's coordinates
    void buildStreetAngles();

    /// @brief Import the graph's adjacency matrix from a file.
    /// If the file is not of a supported format, it will read the file as a matrix with the first two elements being
    /// the number of rows and columns and the following elements being the matrix elements.
    /// @param fileName The name of the file to import the adjacency matrix from.
    /// @param isAdj A boolean value indicating if the file contains the adjacency matrix or the distance matrix.
    /// @throws std::invalid_argument if the file is not found or invalid
    /// The matrix format is deduced from the file extension. Currently only .dsm files are supported.
    void importMatrix(const std::string& fileName, bool isAdj = true);
    /// @brief Import the graph's nodes from a file
    /// @param fileName The name of the file to import the nodes from.
    /// @throws std::invalid_argument if the file is not found, invalid or the format is not supported
    /// @details The file format is deduced from the file extension. Currently only .dsm files are supported.
    ///           The first input number is the number of nodes, followed by the coordinates of each node.
    ///           In the i-th row of the file, the (i - 1)-th node's coordinates are expected.
    void importCoordinates(const std::string& fileName);
    /// @brief Import the graph's nodes from a file
    /// @param fileName The name of the file to import the nodes from.
    /// @throws std::invalid_argument if the file is not found, invalid or the format is not supported
    void importOSMNodes(const std::string& fileName);
    /// @brief Import the graph's streets from a file
    /// @param fileName The name of the file to import the streets from.
    /// @throws std::invalid_argument if the file is not found, invalid or the format is not supported
    void importOSMEdges(const std::string& fileName);

    /// @brief Export the graph's adjacency matrix to a file
    /// @param path The path to the file to export the adjacency matrix to.
    /// @param isAdj A boolean value indicating if the file contains the adjacency matrix or the distance matrix.
    /// @throws std::invalid_argument if the file is not found or invalid
    void exportMatrix(std::string path = "./matrix.dsm", bool isAdj = true);

    /// @brief Add a node to the graph
    /// @param node A std::shared_ptr to the node to add
    void addNode(std::unique_ptr<Node<Id, Size>> node);
    /// @brief Add a node to the graph
    /// @param node A reference to the node to add
    void addNode(const Node<Id, Size>& node);

    template <typename... Tn>
      requires(is_node_v<std::remove_reference_t<Tn>> && ...)
    void addNodes(Tn&&... nodes);

    template <typename T1, typename... Tn>
      requires is_node_v<std::remove_reference_t<T1>> &&
               (is_node_v<std::remove_reference_t<Tn>> && ...)
    void addNodes(T1&& node, Tn&&... nodes);

    /// @brief Convert an existing node to a traffic light
    /// @tparam Delay The type of the traffic light's delay
    /// @param nodeId The id of the node to convert to a traffic light
    /// @throws std::invalid_argument if the node does not exist
    template <typename Delay>
      requires(std::unsigned_integral<Delay>)
    void makeTrafficLight(Id nodeId);

    /// @brief Add a street to the graph
    /// @param street A std::shared_ptr to the street to add
    void addStreet(std::shared_ptr<Street<Id, Size>> street);
    /// @brief Add a street to the graph
    /// @param street A reference to the street to add
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
    const SparseMatrix<Id, bool>& adjMatrix() const;
    /// @brief Get the graph's node map
    /// @return A std::unordered_map containing the graph's nodes
    const std::unordered_map<Id, std::unique_ptr<Node<Id, Size>>>& nodeSet() const;
    /// @brief Get the graph's node map
    /// @return A std::unordered_map containing the graph's nodes
    std::unordered_map<Id, std::unique_ptr<Node<Id, Size>>>& nodeSet();
    /// @brief Get the graph's street map
    /// @return A std::unordered_map containing the graph's streets
    const std::unordered_map<Id, std::unique_ptr<Street<Id, Size>>>& streetSet() const;
    /// @brief Get the graph's street map
    /// @return A std::unordered_map containing the graph's streets
    std::unordered_map<Id, std::unique_ptr<Street<Id, Size>>>& streetSet();
    /// @brief Get a street from the graph
    /// @param source The source node
    /// @param destination The destination node
    /// @return A std::optional containing a std::shared_ptr to the street if it exists, otherwise
    /// std::nullopt
    const std::unique_ptr<Street<Id, Size>>* street(Id source, Id destination) const;

    /// @brief Get the shortest path between two nodes using dijkstra algorithm
    /// @param source The source node
    /// @param destination The destination node
    /// @return A DijkstraResult object containing the path and the distance
    std::optional<DijkstraResult<Id>> shortestPath(
        const Node<Id, Size>& source, const Node<Id, Size>& destination) const;
    /// @brief Get the shortest path between two nodes using dijkstra algorithm
    /// @param source The source node id
    /// @param destination The destination node id
    /// @return A DijkstraResult object containing the path and the distance
    std::optional<DijkstraResult<Id>> shortestPath(Id source, Id destination) const;
  };

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  Graph<Id, Size>::Graph() : m_adjacency{SparseMatrix<Id, bool>()} {}

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  Graph<Id, Size>::Graph(const SparseMatrix<Id, bool>& adj) : m_adjacency{adj} {
    assert(adj.getRowDim() == adj.getColDim());
    auto n{static_cast<Size>(adj.getRowDim())};
    for (const auto& [id, value] : adj) {
      const auto srcId{static_cast<Id>(id / n)};
      const auto dstId{static_cast<Id>(id % n)};
      if (!m_nodes.contains(srcId)) {
        m_nodes.emplace(srcId, std::make_unique<Node<Id, Size>>(srcId));
      }
      if (!m_nodes.contains(dstId)) {
        m_nodes.emplace(dstId, std::make_unique<Node<Id, Size>>(dstId));
      }
      m_streets.emplace(
          id, std::make_unique<Street<Id, Size>>(id, std::make_pair(srcId, dstId)));
    }
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  Graph<Id, Size>::Graph(
      const std::unordered_map<Id, std::unique_ptr<Street<Id, Size>>>& streetSet)
      : m_adjacency{std::make_unique<SparseMatrix<Id, bool>>()} {
    for (const auto& street : streetSet) {
      m_streets.emplace(std::make_pair(street->id(), street));

      Id node1 = street->nodePair().first;
      Id node2 = street->nodePair().second;
      m_nodes.emplace(node1, std::make_unique<Node<Id, Size>>(node1));
      m_nodes.emplace(node2, std::make_unique<Node<Id, Size>>(node2));
    }

    buildAdj();
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Graph<Id, Size>::m_reassignIds() {
    // not sure about this, might need a bit more work
    const auto oldStreetSet{std::move(m_streets)};
    m_streets.clear();
    const auto n{static_cast<Size>(m_nodes.size())};
    std::unordered_map<Id, Id> newStreetIds;
    for (const auto& [streetId, street] : oldStreetSet) {
      const auto srcId{street->nodePair().first};
      const auto dstId{street->nodePair().second};
      const auto newStreetId{static_cast<Id>(srcId * n + dstId)};
      if (m_streets.contains(newStreetId)) {
        throw std::invalid_argument(buildLog("Street with same id already exists."));
      }
      auto newStreet = Street(newStreetId, *street);
      m_streets.emplace(newStreetId, std::make_unique<Street<Id, Size>>(newStreet));
      newStreetIds.emplace(streetId, newStreetId);
    }
    for (const auto& [nodeId, node] : m_nodes) {
      // This is probably not the best way to do this
      if (node->isTrafficLight()) {
        const auto& oldStreetPriorities{node->streetPriorities()};
        std::set<Id> newStreetPriorities;
        for (const auto streetId : oldStreetPriorities) {
          newStreetPriorities.emplace(newStreetIds[streetId]);
        }
        node->setStreetPriorities(newStreetPriorities);
      }
    }
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Graph<Id, Size>::buildAdj() {
    // find max values in streets node pairs
    const auto maxNode{static_cast<Id>(m_nodes.size())};
    m_adjacency.reshape(maxNode, maxNode);
    for (const auto& [streetId, street] : m_streets) {
      m_adjacency.insert(street->nodePair().first, street->nodePair().second, true);
    }
    this->m_reassignIds();
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Graph<Id, Size>::buildStreetAngles() {
    for (const auto& street : m_streets) {
      const auto& node1{m_nodes[street.second->nodePair().first]};
      const auto& node2{m_nodes[street.second->nodePair().second]};
      street.second->setAngle(node1->coords(), node2->coords());
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
      Size rows, cols;
      file >> rows >> cols;
      if (rows != cols) {
        throw std::invalid_argument(buildLog("Adjacency matrix must be square"));
      }
      Size n{rows};
      m_adjacency = SparseMatrix<Id, bool>(n, n);
      // each line has 2 elements
      while (!file.eof()) {
        Id index;
        double val;
        file >> index >> val;
        m_adjacency.insert(index, val);
        const auto srcId{static_cast<Id>(index / n)};
        const auto dstId{static_cast<Id>(index % n)};
        if (!m_nodes.contains(srcId)) {
          m_nodes.emplace(srcId, std::make_unique<Node<Id, Size>>(srcId));
        }
        if (!m_nodes.contains(dstId)) {
          m_nodes.emplace(dstId, std::make_unique<Node<Id, Size>>(dstId));
        }
        m_streets.emplace(
            index,
            std::make_unique<Street<Id, Size>>(index, std::make_pair(srcId, dstId)));
        assert(index == srcId * n + dstId);
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
      Size rows, cols;
      file >> rows >> cols;
      if (rows != cols) {
        throw std::invalid_argument(
            buildLog("Adjacency matrix must be square. Rows: " + std::to_string(rows) +
                     " Cols: " + std::to_string(cols)));
      }
      Size n{rows};
      if (n * n > std::numeric_limits<Id>::max()) {
        throw std::invalid_argument(
            buildLog("Matrix size is too large for the current type of Id."));
      }
      m_adjacency = SparseMatrix<Id, bool>(n, n);
      Id index{0};
      while (!file.eof()) {
        double value;
        file >> value;
        if (value < 0) {
          throw std::invalid_argument(
              buildLog("Adjacency matrix elements must be positive"));
        }
        if (value > 0) {
          m_adjacency.insert(index, true);
          const auto srcId{static_cast<Id>(index / n)};
          const auto dstId{static_cast<Id>(index % n)};
          if (!m_nodes.contains(srcId)) {
            m_nodes.emplace(srcId, std::make_unique<Node<Id, Size>>(srcId));
          }
          if (!m_nodes.contains(dstId)) {
            m_nodes.emplace(dstId, std::make_unique<Node<Id, Size>>(dstId));
          }
          m_streets.emplace(
              index,
              std::make_unique<Street<Id, Size>>(index, std::make_pair(srcId, dstId)));
          assert(index == srcId * n + dstId);
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
  void Graph<Id, Size>::importCoordinates(const std::string& fileName) {
    std::string fileExt = fileName.substr(fileName.find_last_of(".") + 1);
    if (fileExt == "dsm") {
      // first input number is the number of nodes
      std::ifstream file{fileName};
      if (!file.is_open()) {
        throw std::invalid_argument(buildLog("Cannot find file: " + fileName));
      }
      Size n;
      file >> n;
      if (n < m_nodes.size()) {
        throw std::invalid_argument(
            buildLog("Number of node cordinates in file is too small."));
      }
      double lat, lon;
      for (Size i{0}; i < n; ++i) {
        file >> lat >> lon;
        if (m_nodes.contains(i)) {
          m_nodes[i]->setCoords(std::make_pair(lat, lon));
        }
      }
    } else {
      throw std::invalid_argument(buildLog("File extension not supported."));
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
        m_nodes.emplace(nodeIndex,
                        std::make_unique<Node<Id, Size>>(
                            nodeIndex, std::make_pair(std::stod(lat), std::stod(lon))));
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
        m_streets.emplace(streetId,
                          std::make_unique<Street<Id, Size>>(
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
  void Graph<Id, Size>::exportMatrix(std::string path, bool isAdj) {
    std::ofstream file{path};
    if (!file.is_open()) {
      throw std::invalid_argument(buildLog("Cannot open file: " + path));
    }
    if (isAdj) {
      file << m_adjacency.getRowDim() << '\t' << m_adjacency.getColDim() << '\n';
      for (const auto& [id, value] : m_adjacency) {
        file << id << '\t' << value << '\n';
      }
    } else {
      file << m_adjacency.getRowDim() << " " << m_adjacency.getColDim() << '\n';
      for (const auto& [id, street] : m_streets) {
        file << id << '\t' << street->length() << '\n';
      }
    }
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Graph<Id, Size>::addNode(std::unique_ptr<Node<Id, Size>> node) {
    m_nodes.emplace(std::make_pair(node->id(), std::move(node)));
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Graph<Id, Size>::addNode(const Node<Id, Size>& node) {
    m_nodes.emplace(std::make_pair(node.id(), std::make_unique<Node<Id, Size>>(node)));
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
  template <typename Delay>
    requires(std::unsigned_integral<Delay>)
  void Graph<Id, Size>::makeTrafficLight(Id nodeId) {
    if (!m_nodes.contains(nodeId)) {
      throw std::invalid_argument(buildLog("Node does not exist."));
    }
    auto& pNode = m_nodes[nodeId];
    pNode = std::make_unique<TrafficLight<Id, Size, Delay>>(*pNode);
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Graph<Id, Size>::addStreet(std::shared_ptr<Street<Id, Size>> street) {
    if (m_streets.contains(street->id())) {
      throw std::invalid_argument(buildLog("Street with same id already exists."));
    }
    // emplace nodes
    const auto srcId{street.nodePair().first};
    const auto dstId{street.nodePair().second};
    if (!m_nodes.contains(srcId)) {
      m_nodes.emplace(srcId, std::make_unique<Node<Id, Size>>(srcId));
    }
    if (!m_nodes.contains(dstId)) {
      m_nodes.emplace(dstId, std::make_unique<Node<Id, Size>>(dstId));
    }
    // emplace street
    m_streets.emplace(std::make_pair(street->id(), street));
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Graph<Id, Size>::addStreet(const Street<Id, Size>& street) {
    if (m_streets.contains(street.id())) {
      throw std::invalid_argument(buildLog("Street with same id already exists."));
    }
    // emplace nodes
    const auto srcId{street.nodePair().first};
    const auto dstId{street.nodePair().second};
    if (!m_nodes.contains(srcId)) {
      m_nodes.emplace(srcId, std::make_unique<Node<Id, Size>>(srcId));
    }
    if (!m_nodes.contains(dstId)) {
      m_nodes.emplace(dstId, std::make_unique<Node<Id, Size>>(dstId));
    }
    // emplace street
    m_streets.emplace(
        std::make_pair(street.id(), std::make_unique<Street<Id, Size>>(street)));
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  template <typename T1>
    requires is_street_v<std::remove_reference_t<T1>>
  void Graph<Id, Size>::addStreets(T1&& street) {
    if (m_streets.contains(street.id())) {
      throw std::invalid_argument(buildLog("Street with same id already exists."));
    }
    // emplace nodes
    const auto srcId{street.nodePair().first};
    const auto dstId{street.nodePair().second};
    if (!m_nodes.contains(srcId)) {
      m_nodes.emplace(srcId, std::make_unique<Node<Id, Size>>(srcId));
    }
    if (!m_nodes.contains(dstId)) {
      m_nodes.emplace(dstId, std::make_unique<Node<Id, Size>>(dstId));
    }
    // emplace street
    m_streets.emplace(
        std::make_pair(street.id(), std::make_unique<Street<Id, Size>>(street)));
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
  const SparseMatrix<Id, bool>& Graph<Id, Size>::adjMatrix() const {
    return m_adjacency;
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  const std::unordered_map<Id, std::unique_ptr<Node<Id, Size>>>&
  Graph<Id, Size>::nodeSet() const {
    return m_nodes;
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  std::unordered_map<Id, std::unique_ptr<Node<Id, Size>>>& Graph<Id, Size>::nodeSet() {
    return m_nodes;
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  const std::unordered_map<Id, std::unique_ptr<Street<Id, Size>>>&
  Graph<Id, Size>::streetSet() const {
    return m_streets;
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  std::unordered_map<Id, std::unique_ptr<Street<Id, Size>>>& Graph<Id, Size>::streetSet() {
    return m_streets;
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  const std::unique_ptr<Street<Id, Size>>* Graph<Id, Size>::street(Id source,
                                                                   Id destination) const {
    auto streetIt = std::find_if(m_streets.begin(),
                                 m_streets.end(),
                                 [source, destination](const auto& street) -> bool {
                                   return street.second->nodePair().first == source &&
                                          street.second->nodePair().second == destination;
                                 });
    if (streetIt == m_streets.end()) {
      return nullptr;
    }
    Size n = m_nodes.size();
    auto id1 = streetIt->first;
    auto id2 = source * n + destination;
    assert(id1 == id2);
    if (id1 != id2) {
      std::cout << "node size: " << n << std::endl;
      std::cout << "Street id: " << id1 << std::endl;
      std::cout << "Nodes: " << id2 << std::endl;
    }
    return &(streetIt->second);
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  std::optional<DijkstraResult<Id>> Graph<Id, Size>::shortestPath(
      const Node<Id, Size>& source, const Node<Id, Size>& destination) const {
    return this->shortestPath(source.id(), destination.id());
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  std::optional<DijkstraResult<Id>> Graph<Id, Size>::shortestPath(Id source,
                                                                  Id destination) const {
    const Id sourceId{source};

    std::unordered_set<Id> unvisitedNodes;
    bool source_found{false};
    bool dest_found{false};
    std::for_each(m_nodes.begin(),
                  m_nodes.end(),
                  [&unvisitedNodes, &source_found, &dest_found, source, destination](
                      const auto& node) -> void {
                    if (!source_found && node.first == source) {
                      source_found = true;
                    }
                    if (!dest_found && node.first == destination) {
                      dest_found = true;
                    }
                    unvisitedNodes.emplace(node.first);
                  });
    if (!source_found || !dest_found) {
      return std::nullopt;
    }

    const size_t n_nodes{m_nodes.size()};
    auto adj{m_adjacency};

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
      source = *std::min_element(unvisitedNodes.begin(),
                                 unvisitedNodes.end(),
                                 [&dist](const auto& a, const auto& b) -> bool {
                                   return dist[a].second < dist[b].second;
                                 });

      unvisitedNodes.erase(source);
      visitedNodes.emplace(source);

      const auto& neighbors{adj.getRow(source)};
      for (const auto& neighbour : neighbors) {
        // if the node has already been visited, skip it
        if (visitedNodes.find(neighbour.first) != visitedNodes.end()) {
          continue;
        }
        double streetLength = (*(this->street(source, neighbour.first)))->length();
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
