#include "doctest.h"

#include "Graph.hpp"
#include "Train.hpp"
#include "TrainDynamics.hpp"

using Dynamics = dsm::TrainDynamics;
using Graph = dsm::Graph;
using Train = dsm::Train;

TEST_CASE("Train Dynamics") {
  auto graph = Graph{};
  graph.importMatrix("./data/matrix.dsm");
  graph.buildAdj();
  auto trainDynamics = Dynamics{graph, 69};
}