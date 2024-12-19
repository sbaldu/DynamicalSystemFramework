#include "doctest.h"

#include "Graph.hpp"
#include "Train.hpp"
#include "TrainDynamics.hpp"

using TrainDynamics = dsm::TrainDynamics;
using Graph = dsm::Graph;
using Train = dsm::Train;

TEST_CASE("Train Dynamics") {
  auto graph = Graph{};
  graph.importMatrix("./data/matrix.dsm");
  graph.buildAdj();
  auto dynamics = TrainDynamics{graph, 69};
  auto trip = std::vector<dsm::Id>{0, 1, 2, 3, 4};
  auto schedule = std::vector<std::pair<dsm::Time, dsm::Time>>{{0, 10}, {10, 20}, {20, 30}, {30, 40}, {40, 50}};
  dynamics.addTrain(trip, 0, dsm::train_t::R, schedule);
  // dynamics.evolve();
}