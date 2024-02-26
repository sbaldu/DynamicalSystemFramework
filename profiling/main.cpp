#include "dsm/dsm.hpp"
#include <array>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

using unit = uint32_t;

using Graph = dsm::Graph<unit, unit>;
using Itinerary = dsm::Itinerary<unit>;
using Dynamics = dsm::FirstOrderDynamics<unit, unit, unit>;

int main() {
  Graph graph{};
  std::cout << "Importing matrix.dat...\n";
  graph.importMatrix("../test/data/rawMatrix.dat", false);
  std::cout << "Number of nodes: " << graph.nodeSet().size() << '\n'
            << "Number of streets: " << graph.streetSet().size() << '\n';
  for (auto& streetPair : graph.streetSet()) {
    auto street = streetPair.second;
    street->setCapacity(100);
    street->setMaxSpeed(10.);
  }
  for (auto& nodePair : graph.nodeSet()) {
    auto node = nodePair.second;
    node->setCapacity(10);
  }
  std::cout << "Done.\n";

  Itinerary it1{0, 10, 118};
  Itinerary it2{1, 7, 118};
  Itinerary it3{2, 4, 118};
  Itinerary it4{3, 1, 118};
  Itinerary it5{4, 10, 115};
  Itinerary it6{5, 7, 115};
  Itinerary it7{6, 4, 115};
  Itinerary it8{7, 1, 115};
  Itinerary it9{8, 10, 112};
  Itinerary it10{9, 7, 112};
  Itinerary it11{10, 4, 112};
  Itinerary it12{11, 1, 112};
  Itinerary it13{12, 10, 109};
  Itinerary it14{13, 7, 109};
  Itinerary it15{14, 4, 109};
  Itinerary it16{15, 1, 109};

  std::cout << "Creating dynamics...\n";

  Dynamics dynamics{graph};
  dynamics.addItinerary(it1);
  dynamics.addItinerary(it2);
  dynamics.addItinerary(it3);
  dynamics.addItinerary(it4);
  dynamics.addItinerary(it5);
  dynamics.addItinerary(it6);
  dynamics.addItinerary(it7);
  dynamics.addItinerary(it8);
  dynamics.addItinerary(it9);
  dynamics.addItinerary(it10);
  dynamics.addItinerary(it11);
  dynamics.addItinerary(it12);
  dynamics.addItinerary(it13);
  dynamics.addItinerary(it14);
  dynamics.addItinerary(it15);
  dynamics.addItinerary(it16);
  dynamics.setSeed(69);
  dynamics.setErrorProbability(0.3);
  dynamics.setMinSpeedRateo(0.95);
  dynamics.updatePaths();

  std::cout << "Done.\n"
            << "Running simulation...\n";

  for (unsigned int i{0}; i < 1000; ++i) {
    if (i < 12e3) {
      if (i % 60 == 0) {
        dynamics.addRandomAgents(100, true);
      }
    }
    dynamics.evolve(false);
  }
  std::cout << '\n' << "Done.\n";

  return 0;
}
