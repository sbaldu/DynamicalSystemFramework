#include "TrainDynamics.hpp"

namespace dsm {
  TrainDynamics::TrainDynamics(Graph& graph, std::optional<unsigned int> seed)
      : Dynamics<Train>(graph, seed) {}
}  // namespace dsm