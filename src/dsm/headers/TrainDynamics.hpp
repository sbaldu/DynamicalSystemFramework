#pragma once

#include "Dynamics.hpp"
#include "Train.hpp"
#include "../utility/Typedef.hpp"

#include <type_traits>

namespace dsm {
  class TrainDynamics : public Dynamics<Train> {
  private:
    void m_evolveStreet(const std::unique_ptr<Street>& pStreet,
                        bool reinsertAgents) final;
    bool m_evolveNode(const std::unique_ptr<Node>& pNode) final;
    void m_evolveAgents() final;

  public:
    TrainDynamics(Graph& graph, std::optional<unsigned int> seed);

    void setAgentSpeed(Id agentId) final;
    void evolve(bool reinsertAgents = false) final;

    template <typename... TArgs>
      requires(std::is_constructible_v<Train, Id, TArgs...>)
    void addTrain(TArgs&&... args);
  };

  template <typename... TArgs>
    requires(std::is_constructible_v<Train, Id, TArgs...>)
  void TrainDynamics::addTrain(TArgs&&... args) {
    Id id = this->m_agents.empty() ? 0 : this->m_agents.rbegin()->first + 1;
    this->addAgent(0, std::forward<TArgs>(args)...);
  }
}  // namespace dsm