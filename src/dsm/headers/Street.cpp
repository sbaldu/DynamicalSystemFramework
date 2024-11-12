
#include "Street.hpp"

namespace dsm {
  Street::Street(Id id, const Street& street)
      : m_nodePair{street.nodePair()},
        m_len{street.length()},
        m_maxSpeed{street.maxSpeed()},
        m_angle{street.angle()},
        m_id{id},
        m_capacity{street.capacity()},
        m_transportCapacity{street.transportCapacity()},
        m_nLanes{street.nLanes()} {
    for (auto i{0}; i < street.nLanes(); ++i) {
      m_exitQueues.push_back(dsm::queue<Size>());
    }
  }

  Street::Street(Id index, std::pair<Id, Id> pair)
      : m_nodePair{std::move(pair)},
        m_len{1.},
        m_maxSpeed{13.8888888889},
        m_angle{0.},
        m_id{index},
        m_capacity{1},
        m_transportCapacity{1},
        m_nLanes{1} {
    m_exitQueues.push_back(dsm::queue<Size>());
  }

  Street::Street(Id id, Size capacity, double len, std::pair<Id, Id> nodePair)
      : m_nodePair{std::move(nodePair)},
        m_len{len},
        m_maxSpeed{13.8888888889},
        m_angle{0.},
        m_id{id},
        m_capacity{capacity},
        m_transportCapacity{1},
        m_nLanes{1} {
    m_exitQueues.push_back(dsm::queue<Size>());
  }

  Street::Street(
      Id id, Size capacity, double len, double maxSpeed, std::pair<Id, Id> nodePair)
      : m_nodePair{std::move(nodePair)},
        m_len{len},
        m_angle{0.},
        m_id{id},
        m_capacity{capacity},
        m_transportCapacity{1},
        m_nLanes{1} {
    this->setMaxSpeed(maxSpeed);
    m_exitQueues.push_back(dsm::queue<Size>());
  }

  Street::Street(Id id,
                 Size capacity,
                 double len,
                 double maxSpeed,
                 std::pair<Id, Id> nodePair,
                 int16_t nLanes)
      : m_nodePair{std::move(nodePair)},
        m_len{len},
        m_angle{0.},
        m_id{id},
        m_capacity{capacity},
        m_transportCapacity{1}

  {
    this->setMaxSpeed(maxSpeed);
    this->setCapacity(capacity);
    this->setNLanes(nLanes);
    m_exitQueues.resize(nLanes);
    for (auto i{0}; i < nLanes; ++i) {
      m_exitQueues.push_back(dsm::queue<Size>());
    }
  }

  void Street::setLength(double len) {
    if (len < 0.) {
      throw std::invalid_argument(
          buildLog(std::format("The length of a street ({}) cannot be negative.", len)));
    }
    m_len = len;
  }
  void Street::setMaxSpeed(double speed) {
    if (speed < 0.) {
      throw std::invalid_argument(buildLog(
          std::format("The maximum speed of a street ({}) cannot be negative.", speed)));
    }
    m_maxSpeed = speed;
  }
  void Street::setAngle(std::pair<double, double> srcNode,
                        std::pair<double, double> dstNode) {
    // N.B.: lat, lon <==> y, x
    double delta_y{dstNode.first - srcNode.first};
    double delta_x{dstNode.second - srcNode.second};
    double angle{std::atan2(delta_y, delta_x)};
    if (angle < 0.) {
      angle += 2 * std::numbers::pi;
    }
    this->setAngle(angle);
  }
  void Street::setAngle(double angle) {
    if (std::abs(angle) > 2 * std::numbers::pi) {
      throw std::invalid_argument(buildLog(std::format(
          "The angle of a street ({}) must be between - 2 * pi and 2 * pi.", angle)));
    }
    m_angle = angle;
  }
  void Street::setNLanes(const int16_t nLanes) {
    assert(
        (void(std::format("The number of lanes of the street {} must be greater than 0",
                          static_cast<int>(m_id))),
         nLanes > 0));
    m_nLanes = nLanes;
  }

  void Street::addAgent(Id agentId) {
    assert((void("Agent is already on the street."), !m_waitingAgents.contains(agentId)));
    for (auto const& queue : m_exitQueues) {
      for (auto const& id : queue) {
        assert((void("Agent is already in queue."), id != agentId));
      }
    }
    m_waitingAgents.insert(agentId);
    ;
  }
  void Street::enqueue(Id agentId, size_t index) {
    assert((void("Agent is not on the street."), m_waitingAgents.contains(agentId)));
    for (auto const& queue : m_exitQueues) {
      for (auto const& id : queue) {
        assert((void("Agent is already in queue."), id != agentId));
      }
    }
    m_waitingAgents.erase(agentId);
    m_exitQueues[index].push(agentId);
  }
  std::optional<Id> Street::dequeue(size_t index) {
    if (m_exitQueues[index].empty()) {
      return std::nullopt;
    }
    Id id = m_exitQueues[index].front();
    m_exitQueues[index].pop();
    return id;
  }

  Size Street::nAgents() const {
    Size nAgents{static_cast<Size>(m_waitingAgents.size())};
    for (const auto& queue : m_exitQueues) {
      nAgents += queue.size();
    }
    return nAgents;
  }

  Size Street::nExitingAgents() const {
    Size nAgents{0};
    for (const auto& queue : m_exitQueues) {
      nAgents += queue.size();
    }
    return nAgents;
  }

  double Street::deltaAngle(double const previousStreetAngle) const {
    double deltaAngle{m_angle - previousStreetAngle};
    if (deltaAngle > std::numbers::pi) {
      deltaAngle -= 2 * std::numbers::pi;
    } else if (deltaAngle < -std::numbers::pi) {
      deltaAngle += 2 * std::numbers::pi;
    }
    return deltaAngle;
  }

  SpireStreet::SpireStreet(Id id, const Street& street)
      : Street(id, street), m_agentCounterIn{0}, m_agentCounterOut{0} {}

  SpireStreet::SpireStreet(Id id, Size capacity, double len, std::pair<Id, Id> nodePair)
      : Street(id, capacity, len, nodePair), m_agentCounterIn{0}, m_agentCounterOut{0} {}

  SpireStreet::SpireStreet(
      Id id, Size capacity, double len, double maxSpeed, std::pair<Id, Id> nodePair)
      : Street(id, capacity, len, maxSpeed, nodePair),
        m_agentCounterIn{0},
        m_agentCounterOut{0} {}

  void SpireStreet::addAgent(Id agentId) {
    Street::addAgent(agentId);
    ++m_agentCounterIn;
  }

  Size SpireStreet::inputCounts(bool resetValue) {
    if (!resetValue)
      return m_agentCounterIn;
    Size flow = m_agentCounterIn;
    m_agentCounterIn = 0;
    m_agentCounterOut = 0;
    return flow;
  }
  Size SpireStreet::outputCounts(bool resetValue) {
    if (!resetValue)
      return m_agentCounterOut;
    Size flow = m_agentCounterOut;
    m_agentCounterIn = 0;
    m_agentCounterOut = 0;
    return flow;
  }
  int SpireStreet::meanFlow() {
    int flow = static_cast<int>(m_agentCounterIn) - static_cast<int>(m_agentCounterOut);
    m_agentCounterIn = 0;
    m_agentCounterOut = 0;
    return flow;
  }

  std::optional<Id> SpireStreet::dequeue(size_t index) {
    std::optional<Id> id = Street::dequeue(index);
    if (id.has_value()) {
      ++m_agentCounterOut;
    }
    return id;
  }

};  // namespace dsm
