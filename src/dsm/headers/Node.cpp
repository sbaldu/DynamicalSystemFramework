
#include "Node.hpp"

namespace dsm {

  void Intersection::setCapacity(Size capacity) {
    if (capacity < m_agents.size()) {
      throw std::runtime_error(buildLog(
          std::format("Intersection capacity ({}) is smaller than the current queue size ({}).",
                      capacity,
                      m_agents.size())));
    }
    Node::setCapacity(capacity);
  }

  void Intersection::addAgent(double angle, Id agentId) {
    if (m_agents.size() == this->m_capacity) {
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
    if (m_agents.size() == this->m_capacity) {
      throw std::runtime_error(buildLog("Intersection is full."));
    }
    for (auto const [angle, id] : m_agents) {
      if (id == agentId) {
        throw std::runtime_error(
            buildLog(std::format("Agent with id {} is already on the node.", id)));
      }
    }
    int lastKey{0};
    if (!m_agents.empty()) {
      lastKey = m_agents.rbegin()->first + 1;
    }
    m_agents.emplace(lastKey, agentId);
    ++m_agentCounter;
  }

  void Intersection::removeAgent(Id agentId) {
    for (auto it{m_agents.begin()}; it != m_agents.end(); ++it) {
      if (it->second == agentId) {
        m_agents.erase(it);
        return;
      }
    }
    throw std::runtime_error(
        buildLog(std::format("Agent with id {} is not on the node", agentId)));
  }

  Size Intersection::agentCounter() {
    Size copy{m_agentCounter};
    m_agentCounter = 0;
    return copy;
  }

  Roundabout::Roundabout(const Node& node) : Node{node.id()} {
    if (node.coords().has_value()) {
      this->setCoords(node.coords().value());
    }
    this->setCapacity(node.capacity());
  }

  void Roundabout::enqueue(Id agentId) {
    if (m_agents.size() == this->m_capacity) {
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
