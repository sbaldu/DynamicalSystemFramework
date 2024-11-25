
#include "Graph.hpp"

namespace dsm {
  Graph::Graph()
      : m_adjacency{SparseMatrix<bool>()},
        m_maxAgentCapacity{std::numeric_limits<unsigned long long>::max()} {}

  Graph::Graph(const SparseMatrix<bool>& adj)
      : m_adjacency{adj},
        m_maxAgentCapacity{std::numeric_limits<unsigned long long>::max()} {
    assert(adj.getRowDim() == adj.getColDim());
    auto n{static_cast<Size>(adj.getRowDim())};
    for (const auto& [id, value] : adj) {
      const auto srcId{static_cast<Id>(id / n)};
      const auto dstId{static_cast<Id>(id % n)};
      if (!m_nodes.contains(srcId)) {
        m_nodes.emplace(srcId, std::make_unique<Intersection>(srcId));
      }
      if (!m_nodes.contains(dstId)) {
        m_nodes.emplace(dstId, std::make_unique<Intersection>(dstId));
      }
      m_streets.emplace(id, std::make_unique<Street>(id, std::make_pair(srcId, dstId)));
    }
  }

  Graph::Graph(const std::unordered_map<Id, std::unique_ptr<Street>>& streetSet)
      : m_adjacency{SparseMatrix<bool>()} {
    for (auto& street : streetSet) {
      m_streets.emplace(street.second->id(), street.second.get());

      Id node1 = street.second->nodePair().first;
      Id node2 = street.second->nodePair().second;
      m_nodes.emplace(node1, std::make_unique<Intersection>(node1));
      m_nodes.emplace(node2, std::make_unique<Intersection>(node2));
    }

    buildAdj();
  }

  void Graph::m_reassignIds() {
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
      if (street->isSpire()) {
        m_streets.emplace(
            newStreetId,
            std::make_unique<SpireStreet>(SpireStreet{newStreetId, *street}));
      } else {
        m_streets.emplace(newStreetId,
                          std::make_unique<Street>(Street{newStreetId, *street}));
      }
      newStreetIds.emplace(streetId, newStreetId);
    }
    for (const auto& [nodeId, node] : m_nodes) {
      // This is probably not the best way to do this
      if (node->isIntersection()) {
        auto& intersection = dynamic_cast<Intersection&>(*node);
        const auto& oldStreetPriorities{intersection.streetPriorities()};
        std::set<Id> newStreetPriorities;
        for (const auto streetId : oldStreetPriorities) {
          newStreetPriorities.emplace(newStreetIds[streetId]);
        }
        intersection.setStreetPriorities(newStreetPriorities);
      }
    }
  }

  void Graph::m_setStreetAngles() {
    for (const auto& [streetId, street] : m_streets) {
      const auto& srcNode{m_nodes[street->nodePair().first]};
      const auto& dstNode{m_nodes[street->nodePair().second]};
      if (srcNode->coords().has_value() && dstNode->coords().has_value()) {
        street->setAngle(srcNode->coords().value(), dstNode->coords().value());
      }
    }
  }

  void Graph::buildAdj() {
    // find max values in streets node pairs
    m_maxAgentCapacity = 0;
    const auto maxNode{static_cast<Id>(m_nodes.size())};
    m_adjacency.reshape(maxNode, maxNode);
    for (const auto& [streetId, street] : m_streets) {
      m_maxAgentCapacity += street->capacity();
      m_adjacency.insert(street->nodePair().first, street->nodePair().second, true);
    }
    this->m_reassignIds();
    this->m_setStreetAngles();
  }

  void Graph::buildStreetAngles() {
    for (const auto& street : m_streets) {
      const auto& node1{m_nodes[street.second->nodePair().first]};
      const auto& node2{m_nodes[street.second->nodePair().second]};
      street.second->setAngle(node1->coords().value(), node2->coords().value());
    }
  }

  void Graph::adjustNodeCapacities() {
    int16_t value;
    for (Id nodeId = 0; nodeId < m_nodes.size(); ++nodeId) {
      value = 0;
      for (const auto& [streetId, _] : m_adjacency.getCol(nodeId, true)) {
        value += m_streets[streetId]->nLanes() * m_streets[streetId]->transportCapacity();
      }
      m_nodes[nodeId]->setCapacity(value);
      value = 0;
      for (const auto& [streetId, _] : m_adjacency.getRow(nodeId, true)) {
        value += m_streets[streetId]->nLanes() * m_streets[streetId]->transportCapacity();
      }
      m_nodes[nodeId]->setTransportCapacity(value);
      if (m_nodes[nodeId]->capacity() == 0) {
        m_nodes[nodeId]->setCapacity(value);
      }
    }
  }

  void Graph::normalizeStreetCapacities(double meanVehicleLength) {
    m_maxAgentCapacity = 0;
    for (const auto& [_, street] : m_streets) {
      auto const maxCapacity{
          static_cast<Size>(street->length() * street->nLanes() / meanVehicleLength)};
      m_maxAgentCapacity += maxCapacity;
      street->setCapacity(maxCapacity);
    }
  }

  void Graph::importMatrix(const std::string& fileName, bool isAdj) {
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
      m_adjacency = SparseMatrix<bool>(n, n);
      // each line has 2 elements
      while (!file.eof()) {
        Id index;
        double val;
        file >> index >> val;
        m_adjacency.insert(index, val);
        const auto srcId{static_cast<Id>(index / n)};
        const auto dstId{static_cast<Id>(index % n)};
        if (!m_nodes.contains(srcId)) {
          m_nodes.emplace(srcId, std::make_unique<Intersection>(srcId));
        }
        if (!m_nodes.contains(dstId)) {
          m_nodes.emplace(dstId, std::make_unique<Intersection>(dstId));
        }
        m_streets.emplace(index,
                          std::make_unique<Street>(index, std::make_pair(srcId, dstId)));
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
      m_adjacency = SparseMatrix<bool>(n, n);
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
            m_nodes.emplace(srcId, std::make_unique<Intersection>(srcId));
          }
          if (!m_nodes.contains(dstId)) {
            m_nodes.emplace(dstId, std::make_unique<Intersection>(dstId));
          }
          m_streets.emplace(
              index, std::make_unique<Street>(index, std::make_pair(srcId, dstId)));
          assert(index == srcId * n + dstId);
          if (!isAdj) {
            m_streets[index]->setLength(value);
          }
        }
        ++index;
      }
    }
  }

  void Graph::importCoordinates(const std::string& fileName) {
    std::string fileExt = fileName.substr(fileName.find_last_of(".") + 1);
    if (fileExt == "dsm") {
      // first input number is the number of nodes
      std::ifstream file{fileName};
      assert((void("Coordinates file not found."), file.is_open()));
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
    } else if (fileExt == "csv") {
      std::ifstream ifs{fileName};
      assert((void("Coordinates file not found."), ifs.is_open()));
      // Check if the first line is nodeId;lat;lon
      std::string line;
      std::getline(ifs, line);
      assert((void("Invalid file format."), line == "nodeId;lat;lon"));
      double dLat, dLon;
      while (!ifs.eof()) {
        std::getline(ifs, line);
        if (line.empty()) {
          continue;
        }
        std::istringstream iss{line};
        std::string nodeId, lat, lon;
        std::getline(iss, nodeId, ';');
        std::getline(iss, lat, ';');
        std::getline(iss, lon, '\n');
        dLat = lat == "Nan" ? 0. : std::stod(lat);
        dLon = lon == "Nan" ? 0. : std::stod(lon);
        if (m_nodes.contains(std::stoul(nodeId))) {
          m_nodes[std::stoul(nodeId)]->setCoords(std::make_pair(dLat, dLon));
        } else {
          std::cerr << std::format("WARNING: Node with id {} not found.", nodeId)
                    << std::endl;
        }
      }
    } else {
      throw std::invalid_argument(buildLog("File extension not supported."));
    }
  }

  void Graph::importOSMNodes(const std::string& fileName) {
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
                        std::make_unique<Intersection>(
                            nodeIndex, std::make_pair(std::stod(lat), std::stod(lon))));
        m_nodeMapping.emplace(std::make_pair(nodeId, nodeIndex));
        ++nodeIndex;
      }
    } else {
      throw std::invalid_argument(buildLog("File extension not supported"));
    }
  }

  void Graph::importOSMEdges(const std::string& fileName) {
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
        std::string sourceId, targetId, length, oneway, lanes, highway, maxspeed, bridge;
        // u;v;length;oneway;highway;maxspeed;bridge
        std::getline(iss, sourceId, ';');
        std::getline(iss, targetId, ';');
        std::getline(iss, length, ';');
        std::getline(iss, oneway, ';');
        std::getline(iss, lanes, ';');
        std::getline(iss, highway, ';');
        std::getline(iss, maxspeed, ';');
        std::getline(iss, bridge, ';');
        try {
          std::stod(maxspeed);
        } catch (const std::invalid_argument& e) {
          maxspeed = "30";
        }

        uint8_t numLanes;
        if (lanes.empty()) {
          numLanes = 1;  // Default to 1 lane if no value is provided
        } else {
          try {
            // Convert lanes to a double first, then cast to uint8_t
            double lanesVal = std::stod(lanes);
            if (lanesVal < 1 || std::isnan(lanesVal)) {
              numLanes = 1;  // Default to 1 if lanes is invalid
            } else {
              numLanes = static_cast<uint8_t>(lanesVal);  // Cast to uint8_t
            }
          } catch (const std::invalid_argument&) {
            numLanes = 1;  // Default to 1 if conversion fails
          }
        }

        Id streetId = std::stoul(sourceId) + std::stoul(targetId) * m_nodes.size();
        m_streets.emplace(
            streetId,
            std::make_unique<Street>(streetId,
                                     1,
                                     std::stod(maxspeed),
                                     std::stod(length),
                                     std::make_pair(m_nodeMapping[std::stoul(sourceId)],
                                                    m_nodeMapping[std::stoul(targetId)]),
                                     numLanes));
      }
    } else {
      std::string errrorMsg{"Error at line " + std::to_string(__LINE__) + " in file " +
                            __FILE__ + ": " + "File extension not supported"};
      throw std::invalid_argument(errrorMsg);
    }
  }

  void Graph::exportMatrix(std::string path, bool isAdj) {
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

  void Graph::exportCoordinates(std::string const& path) {
    // assert that path ends with ".csv"
    assert((void("Only csv export is supported."),
            path.substr(path.find_last_of(".")) == ".csv"));
    std::ofstream file{path};
    // Column names
    file << "nodeId;lat;lon\n";
    for (const auto& [id, node] : m_nodes) {
      file << id << ';';
      if (node->coords().has_value()) {
        file << node->coords().value().first << ';' << node->coords().value().second;
      } else {
        file << "Nan;Nan";
      }
      file << '\n';
    }
    file.close();
  }

  void Graph::addNode(std::unique_ptr<Node> node) {
    m_nodes.emplace(std::make_pair(node->id(), std::move(node)));
  }

  void Graph::addNode(const Intersection& node) {
    m_nodes.emplace(std::make_pair(node.id(), std::make_unique<Intersection>(node)));
  }

  Roundabout& Graph::makeRoundabout(Id nodeId) {
    if (!m_nodes.contains(nodeId)) {
      throw std::invalid_argument(buildLog("Node does not exist."));
    }
    auto& pNode = m_nodes[nodeId];
    pNode = std::make_unique<Roundabout>(*pNode);
    return dynamic_cast<Roundabout&>(*pNode);
  }
  SpireStreet& Graph::makeSpireStreet(Id streetId) {
    if (!m_streets.contains(streetId)) {
      throw std::invalid_argument(
          buildLog(std::format("Street with id {} does not exist.", streetId)));
    }
    auto& pStreet = m_streets[streetId];
    pStreet = std::make_unique<SpireStreet>(pStreet->id(), *pStreet);
    return dynamic_cast<SpireStreet&>(*pStreet);
  }

  void Graph::addStreet(std::shared_ptr<Street> street) {
    if (m_streets.contains(street->id())) {
      throw std::invalid_argument(
          buildLog(std::format("Street with id {} already exists.", street->id())));
    }
    // emplace nodes
    const auto srcId{street->nodePair().first};
    const auto dstId{street->nodePair().second};
    if (!m_nodes.contains(srcId)) {
      m_nodes.emplace(srcId, std::make_unique<Intersection>(srcId));
    }
    if (!m_nodes.contains(dstId)) {
      m_nodes.emplace(dstId, std::make_unique<Intersection>(dstId));
    }
    // emplace street
    m_streets.emplace(street->id(), street.get());
  }

  void Graph::addStreet(const Street& street) {
    if (m_streets.contains(street.id())) {
      throw std::invalid_argument(
          buildLog(std::format("Street with id {} already exists.", street.id())));
    }
    // emplace nodes
    const auto srcId{street.nodePair().first};
    const auto dstId{street.nodePair().second};
    if (!m_nodes.contains(srcId)) {
      m_nodes.emplace(srcId, std::make_unique<Intersection>(srcId));
    }
    if (!m_nodes.contains(dstId)) {
      m_nodes.emplace(dstId, std::make_unique<Intersection>(dstId));
    }
    // emplace street
    m_streets.emplace(std::make_pair(street.id(), std::make_unique<Street>(street)));
  }

  const std::unique_ptr<Street>* Graph::street(Id source, Id destination) const {
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

  const std::unique_ptr<Street>* Graph::oppositeStreet(Id streetId) const {
    if (!m_streets.contains(streetId)) {
      throw std::invalid_argument(
          buildLog(std::format("Street with id {} does not exist: maybe it has changed "
                               "id once called buildAdj.",
                               streetId)));
    }
    const auto& nodePair = m_streets.at(streetId)->nodePair();
    return this->street(nodePair.second, nodePair.first);
  }

  std::optional<DijkstraResult> Graph::shortestPath(
      const Intersection& source, const Intersection& destination) const {
    return this->shortestPath(source.id(), destination.id());
  }

  std::optional<DijkstraResult> Graph::shortestPath(Id source, Id destination) const {
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
    return DijkstraResult(path, prev[destination].second);
  }

};  // namespace dsm
