#include "TrainDynamics.hpp"

namespace dsm {
  TrainDynamics::TrainDynamics(Graph& graph, std::optional<unsigned int> seed)
      : Dynamics<Train>(graph, seed) {}

  void TrainDynamics::m_evolveStreet(const std::unique_ptr<Street>& pStreet,
                                     bool reinsertAgents) {}

  bool TrainDynamics::m_evolveNode(const std::unique_ptr<Node>& pNode) {
    auto& station = dynamic_cast<Station&>(*pNode);
    if (!station.isActive(m_time)) {
      return false;
    }
    auto optTrainId = station.dequeue();
    if (!optTrainId.has_value()) {
      return false;
    }
    auto const& pTrain = m_agents[optTrainId.value()];
    auto const& possibleMoves = m_itineraries[pTrain->itineraryId()]->path().getRow(pTrain->itineraryId());
    assert(possibleMoves.size() == 1);
    auto const& nextStreetId = possibleMoves.begin()->first;
    auto const& pNextStreet = m_graph.streetSet().at(nextStreetId);
    pNextStreet->addAgent(pTrain->id());
    
    return true; 
  }

  void TrainDynamics::m_evolveAgents() {}

  void TrainDynamics::setAgentSpeed(Id agentId) {}
  void TrainDynamics::evolve(bool reinsertAgents) {
    for (auto const& [_, pNode] : this->m_graph.nodeSet()) {
      this->m_evolveNode(pNode);
    }
    for (auto const& [_, pStreet] : this->m_graph.streetSet()) {
      this->m_evolveStreet(pStreet, reinsertAgents);
    }
  }
}  // namespace dsm