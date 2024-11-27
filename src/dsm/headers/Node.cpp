
#include "Node.hpp"

#include <cassert>

namespace dsm {

  void Intersection::setCapacity(Size capacity) {
    if (capacity < m_agents.size()) {
      throw std::runtime_error(buildLog(std::format(
          "Intersection capacity ({}) is smaller than the current queue size ({}).",
          capacity,
          m_agents.size())));
    }
    Node::setCapacity(capacity);
  }

  void Intersection::addAgent(double angle, Id agentId) {
    if (isFull()) {
      throw std::runtime_error(buildLog("Intersection is full."));
    }
    for (auto const [angle, id] : m_agents) {
      if (id == agentId) {
        throw std::runtime_error(
            buildLog(std::format("Agent with id {} is already on the node.", agentId)));
      }
    }
    auto iAngle{static_cast<int16_t>(angle * 100)};
    m_agents.emplace(iAngle, agentId);
    ++m_agentCounter;
  }

  void Intersection::addAgent(Id agentId) {
    int lastKey{0};
    if (!m_agents.empty()) {
      lastKey = m_agents.rbegin()->first + 1;
    }
    addAgent(static_cast<double>(lastKey), agentId);
  }

  void Intersection::removeAgent(Id agentId) {
    std::erase_if(m_agents,
                  [agentId](const auto& pair) { return pair.second == agentId; });
  }

  Size Intersection::agentCounter() {
    Size copy{m_agentCounter};
    m_agentCounter = 0;
    return copy;
  }

  bool TrafficLightCycle::isGreen(Delay const cycleTime, Delay const counter) const {
    auto const rest{(m_phase + m_greenTime) / cycleTime};
    if (rest) {
      return (counter < rest) || (counter >= m_phase);
    }
    return (counter >= m_phase) && (counter < m_phase + m_greenTime);
  }

  void TrafficLight::setCycle(Id const streetId,
                              Direction direction,
                              TrafficLightCycle const& cycle) {
    if (!(cycle.greenTime() < m_cycleTime)) {
      throw std::invalid_argument(
          buildLog("Green time must be less than the cycle time."));
    }
    if (!(cycle.phase() < m_cycleTime)) {
      throw std::invalid_argument(buildLog("Phase must be less than the cycle time."));
    }
    if (direction == Direction::UTURN) {
      direction = Direction::LEFT;
    }
    if (!m_cycles.contains(streetId)) {
      TrafficLightCycle defaultCycle(m_cycleTime, 0);
      std::vector<TrafficLightCycle> cycles{defaultCycle, defaultCycle, defaultCycle};
      m_cycles.emplace(streetId, cycles);
    }
    m_cycles[streetId][direction] = cycle;
  }

  bool TrafficLight::isGreen(Id const streetId, Direction direction) const {
    if (!m_cycles.contains(streetId)) {
      throw std::invalid_argument(buildLog("Street id does not exist."));
    }
    switch (direction) {
      case Direction::UTURN:
        direction = Direction::LEFT;
        break;
      case Direction::RIGHTANDSTRAIGHT:
        return m_cycles.at(streetId)[Direction::RIGHT].isGreen(m_cycleTime, m_counter) &&
               m_cycles.at(streetId)[Direction::STRAIGHT].isGreen(m_cycleTime, m_counter);
      case Direction::LEFTANDSTRAIGHT:
        return m_cycles.at(streetId)[Direction::LEFT].isGreen(m_cycleTime, m_counter) &&
               m_cycles.at(streetId)[Direction::STRAIGHT].isGreen(m_cycleTime, m_counter);
      case Direction::ANY:
        return m_cycles.at(streetId)[Direction::RIGHT].isGreen(m_cycleTime, m_counter) &&
               m_cycles.at(streetId)[Direction::STRAIGHT].isGreen(m_cycleTime,
                                                                  m_counter) &&
               m_cycles.at(streetId)[Direction::LEFT].isGreen(m_cycleTime, m_counter);
      default:
        break;
    }
    return m_cycles.at(streetId)[direction].isGreen(m_cycleTime, m_counter);
  }

  Roundabout::Roundabout(const Node& node) : Node{node.id()} {
    if (node.coords().has_value()) {
      this->setCoords(node.coords().value());
    }
    this->setCapacity(node.capacity());
  }

  void Roundabout::enqueue(Id agentId) {
    if (isFull()) {
      throw std::runtime_error(buildLog("Roundabout is full."));
    }
    for (const auto id : m_agents) {
      if (id == agentId) {
        throw std::runtime_error(buildLog(
            std::format("Agent with id {} is already on the roundabout.", agentId)));
      }
    }
    m_agents.push(agentId);
  }

  Id Roundabout::dequeue() {
    if (m_agents.empty()) {
      throw std::runtime_error(buildLog("Roundabout is empty."));
    }
    Id agentId{m_agents.front()};
    m_agents.pop();
    return agentId;
  }

};  // namespace dsm
