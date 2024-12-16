#include "TrainDynamics.hpp"

namespace dsm {
  TrainDynamics::TrainDynamics(Graph& graph, std::optional<unsigned int> seed)
      : Dynamics<Train>(graph, seed) {}

    void TrainDynamics::m_evolveStreet(const std::unique_ptr<Street>& pStreet, bool reinsertAgents) {
    }

    bool TrainDynamics::m_evolveNode(const std::unique_ptr<Node>& pNode) {
    }

    void TrainDynamics::m_evolveAgents() {
    }
    
    void TrainDynamics::setAgentSpeed(Id agentId) {
    }
    void TrainDynamics::evolve(bool reinsertAgents) {
    }
}  // namespace dsm