#pragma once

#include "Dynamics.hpp"
#include "Train.hpp"

namespace dsm {
  class TrainDynamics : public Dynamics<Train> {
  private:
  public:
    TrainDynamics(Graph& graph, std::optional<unsigned int> seed);
  };
}  // namespace dsm