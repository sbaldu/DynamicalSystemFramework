
#include "Node.hpp"

#include <cassert>

namespace dsm {

  void Intersection::setCapacity(Size capacity) {
    if (capacity < m_agents.size()) {
      pConsoleLogger->critical(
          "New capacity ({}) is smaller than the current agent size ({}).",
          capacity,
          m_agents.size());
      std::abort();
    }
    Node::setCapacity(capacity);
  }

  void Intersection::addAgent(double angle, Id agentId) {
    if (isFull()) {
      pConsoleLogger->critical("Intersection is full.");
      std::abort();
    }
    for (auto const [angle, id] : m_agents) {
      if (id == agentId) {
        pConsoleLogger->critical("Agent with id {} is already on the node.", agentId);
        std::abort();
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

  Roundabout::Roundabout(const Node& node) : Node{node.id()} {
    if (node.coords().has_value()) {
      this->setCoords(node.coords().value());
    }
    this->setCapacity(node.capacity());
  }

  void Roundabout::enqueue(Id agentId) {
    if (isFull()) {
      pConsoleLogger->critical("Roundabout is full.");
      std::abort();
    }
    for (const auto id : m_agents) {
      if (id == agentId) {
        pConsoleLogger->critical("Agent with id {} is already on the roundabout.",
                                 agentId);
        std::abort();
      }
    }
    m_agents.push(agentId);
  }

  Id Roundabout::dequeue() {
    if (m_agents.empty()) {
      pConsoleLogger->critical("Roundabout is empty.");
      std::abort();
    }
    Id agentId{m_agents.front()};
    m_agents.pop();
    return agentId;
  }

};  // namespace dsm
