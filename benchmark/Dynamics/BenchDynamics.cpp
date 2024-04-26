#include <cstdint>

#include "Graph.hpp"
#include "Itinerary.hpp"
#include "Dynamics.hpp"
#include "Bench.hpp"

using Graph = dsm::Graph<uint32_t, uint32_t>;
using Itinerary = dsm::Itinerary<uint32_t>;
using Dynamics = dsm::FirstOrderDynamics<uint32_t, uint32_t, uint32_t>;

using Bench = sb::Bench<long long int>;

int main() {
  Graph graph{};
  graph.importMatrix("../test/data/matrix.dat", false);
  for (const auto& [streetId, street] : graph.streetSet()) {
    street->setMaxSpeed(13.9);
  }

  Itinerary it1{0, 118};
  Itinerary it2{4, 115};
  Itinerary it3{8, 112};
  Itinerary it4{12, 109};

  Dynamics dynamics{graph};
  dynamics.addItinerary(it1);
  dynamics.addItinerary(it2);
  dynamics.addItinerary(it3);
  dynamics.addItinerary(it4);
  dynamics.setSeed(69);
  dynamics.setErrorProbability(0.3);
  dynamics.setMinSpeedRateo(0.95);

  const int n_rep{100};
  Bench b1(n_rep);
  std::cout << "Benchmarking updatePaths\n";
  dynamics.updatePaths();
  b1.benchmark([&dynamics]() -> void { dynamics.updatePaths(); });
  b1.print<sb::milliseconds>();
}