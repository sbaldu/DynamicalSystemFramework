#pragma once

#include "Dynamics.hpp"
#include "Train.hpp"

namespace dsm {
  class TrainDynamics : public Dynamics<Train> {
  private:
    void m_evolveStreet(const std::unique_ptr<Street>& pStreet, bool reinsertAgents) final;
    bool m_evolveNode(const std::unique_ptr<Node>& pNode) final;
    void m_evolveAgents() final;
  public:
    TrainDynamics(Graph& graph, std::optional<unsigned int> seed);

    void setAgentSpeed(Id agentId) final;
    void evolve(bool reinsertAgents = false) final;
  };
}  // namespace dsm