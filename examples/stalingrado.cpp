/**
 * @brief Main function to simulate traffic dynamics over Via Stalingrado, in Bologna.
 */

#include "../src/dsm/dsm.hpp"
#include <array>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>

#include <thread>
#include <atomic>

std::atomic<unsigned int> progress{0};

using Unit = unsigned int;
using Delay = uint8_t;

using Graph = dsm::Graph;
using Itinerary = dsm::Itinerary;
using Dynamics = dsm::FirstOrderDynamics;
using Street = dsm::Street;
using SpireStreet = dsm::SpireStreet;
using TrafficLight = dsm::TrafficLight;

void printLoadingBar(int const i, int const n) {
  std::cout << "Loading: " << std::setprecision(2) << std::fixed << (i * 100. / n) << "%"
            << '\r';
  std::cout.flush();
}

int main() {
  // Import input data
  std::ifstream ifs{"./data/stalingrado_input.txt"};
  Unit timeUnit{0};
  ifs >> timeUnit;
  std::vector<Unit> vehiclesToInsert{};
  while (!ifs.eof()) {
    Unit vehicleId{0};
    ifs >> vehicleId;
    vehiclesToInsert.push_back(vehicleId);
  }
  const auto MAX_TIME{static_cast<Unit>(timeUnit * vehiclesToInsert.size())};

  // Create the graph

  // Street(StreetId, Capacity, Length, vMax, (from, to))
  Street s01{1, 2281 / 8, 2281., 13.9, std::make_pair(0, 1)};
  Street s12{7, 118 / 8, 118., 13.9, std::make_pair(1, 2)};
  Street s23{13, 222 / 8, 222., 13.9, std::make_pair(2, 3)};
  Street s34{19, 651 / 4, 651., 13.9, std::make_pair(3, 4)};
  // Viale Aldo Moro
  TrafficLight tl1{1, 132};
  tl1.setCycle(s01.id(), dsm::Direction::ANY, {62, 0});
  tl1.setCapacity(1);
  // Via Donato Creti
  TrafficLight tl2{2, 141};
  tl2.setCycle(s12.id(), dsm::Direction::ANY, {72, 0});
  tl2.setCapacity(1);
  // Via del Lavoro
  TrafficLight tl3{3, 138};
  tl3.setCycle(s23.id(), dsm::Direction::ANY, {88, 0});
  tl3.setCapacity(1);
  // Viali
  TrafficLight tl4{4, 131};
  tl4.setCycle(s34.id(), dsm::Direction::ANY, {81, 0});
  tl4.setCapacity(1);

  Graph graph;
  graph.addNode(std::make_unique<TrafficLight>(tl1));
  graph.addNode(std::make_unique<TrafficLight>(tl2));
  graph.addNode(std::make_unique<TrafficLight>(tl3));
  graph.addNode(std::make_unique<TrafficLight>(tl4));
  graph.addStreets(s01, s12, s23, s34);
  graph.buildAdj();
  auto& spire = graph.makeSpireStreet(19);

  std::cout << "Intersections: " << graph.nNodes() << '\n';
  std::cout << "Streets: " << graph.nEdges() << '\n';

  // Create the dynamics
  Dynamics dynamics{graph, 69, 0.95};
  dynamics.setSpeedFluctuationSTD(0.2);
  Itinerary itinerary{0, 4};
  dynamics.addItinerary(itinerary);
  dynamics.updatePaths();

  // lauch progress bar
  std::jthread t([MAX_TIME]() {
    while (progress < MAX_TIME) {
      printLoadingBar(progress, MAX_TIME);
      std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    }
  });
  // Evolution
  auto it = vehiclesToInsert.begin();
  std::ofstream ofs{"./stalingrado_output.csv"};
  // print two columns, time and vehicles
  ofs << "time;vehicle_flux" << '\n';
  while (progress < MAX_TIME) {
    if (progress % 60 == 0) {
      if (progress != 0) {
        ++it;
      }
      if (progress % 300 == 0) {
        ofs << progress << ';' << spire.outputCounts(true) << std::endl;
      }
      dynamics.addAgents(0, *it / 2, 0);
    }
    dynamics.evolve(false);
    ++progress;
  }

  return 0;
}
