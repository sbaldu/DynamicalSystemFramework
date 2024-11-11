
#include "Street.hpp"

namespace dsm {
  Street::Street(Id id, const Street& street)
      : m_nodePair{street.nodePair()},
        m_len{street.length()},
        m_maxSpeed{street.maxSpeed()},
        m_angle{street.angle()},
        m_id{id},
        m_capacity{street.capacity()},
        m_transportCapacity{street.transportCapacity()} {}

  Street::Street(Id index, std::pair<Id, Id> pair)
      : m_nodePair{std::move(pair)},
        m_len{1.},
        m_maxSpeed{13.8888888889},
        m_angle{0.},
        m_id{index},
        m_capacity{1},
        m_transportCapacity{std::numeric_limits<Size>::max()} {}

  Street::Street(Id id, Size capacity, double len, std::pair<Id, Id> nodePair)
      : m_nodePair{std::move(nodePair)},
        m_len{len},
        m_maxSpeed{13.8888888889},
        m_angle{0.},
        m_id{id},
        m_capacity{capacity},
        m_transportCapacity{std::numeric_limits<Size>::max()} {}

  Street::Street(
      Id id, Size capacity, double len, double maxSpeed, std::pair<Id, Id> nodePair)
      : m_nodePair{std::move(nodePair)},
        m_len{len},
        m_angle{0.},
        m_id{id},
        m_capacity{capacity},
        m_transportCapacity{std::numeric_limits<Size>::max()} {
    this->setMaxSpeed(maxSpeed);
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

  void Street::addAgent(Id agentId) {
    if (m_waitingAgents.contains(agentId)) {
      throw std::runtime_error(
          buildLog(std::format("Agent with id {} is already on the street.", agentId)));
    }
    for (auto const& id : m_exitQueue) {
      if (id == agentId) {
        throw std::runtime_error(
            buildLog(std::format("Agent with id {} is already on the street.", agentId)));
      }
    }
    m_waitingAgents.insert(agentId);
  }
  void Street::enqueue(Id agentId) {
    if (!m_waitingAgents.contains(agentId)) {
      throw std::runtime_error(
          buildLog(std::format("Agent with id {} is not on the street.", agentId)));
    }
    for (auto const& id : m_exitQueue) {
      if (id == agentId) {
        throw std::runtime_error(
            buildLog(std::format("Agent with id {} is already on the street.", agentId)));
      }
    }
    m_waitingAgents.erase(agentId);
    m_exitQueue.push(agentId);
  }
  std::optional<Id> Street::dequeue() {
    if (m_exitQueue.empty()) {
      return std::nullopt;
    }
    Id id = m_exitQueue.front();
    m_exitQueue.pop();
    return id;
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

  std::optional<Id> SpireStreet::dequeue() {
    std::optional<Id> id = Street::dequeue();
    if (id.has_value()) {
      ++m_agentCounterOut;
    }
    return id;
  }

};  // namespace dsm
