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

using unit = uint32_t;

std::atomic<unit> progress{0};

using Graph = dsm::Graph<unit, unit>;
using Itinerary = dsm::Itinerary<unit>;
using Dynamics = dsm::FirstOrderDynamics<unit, unit, unit>;
using TrafficLight = dsm::TrafficLight<unit, unit, unit>;
using Street = dsm::Street<unit, unit>;
using SpireStreet = dsm::SpireStreet<unit, unit>;

void printLoadingBar(int const i, int const n) {
  std::cout << "Loading: " << std::setprecision(2) << std::fixed << (i * 100. / n) << "%"
            << '\r';
  std::cout.flush();
}

int main() {
  // Import input data
  std::ifstream ifs{"./data/stalingrado_input.txt"};
  unit timeUnit{0};
  ifs >> timeUnit;
  std::vector<unit> vehiclesToInsert{};
  while (!ifs.eof()) {
    unit vehicleId{0};
    ifs >> vehicleId;
    vehiclesToInsert.push_back(vehicleId);
  }
  const auto MAX_TIME{static_cast<unit>(timeUnit * vehiclesToInsert.size())};

  // Create the graph
  TrafficLight tl1{1}, tl2{2}, tl3{3}, tl4{4};

  // Street(StreetId, Capacity, Length, vMax, (from, to))
  Street s01{1, 2281 / 8, 2281., 13.9, std::make_pair(0, 1)};
  Street s12{7, 118 / 8, 118., 13.9, std::make_pair(1, 2)};
  Street s23{13, 222 / 8, 222., 13.9, std::make_pair(2, 3)};
  Street s34{19, 651 / 4, 651., 13.9, std::make_pair(3, 4)};
  // Viale Aldo Moro
  tl1.setDelay(std::make_pair(62, 70));  // 40, 70
  tl1.setCapacity(1);
  tl1.addStreetPriority(s01.id());
  // Via Donato Creti
  tl2.setDelay(std::make_pair(72, 69));  // 50, 75
  tl2.setCapacity(1);
  tl2.addStreetPriority(s12.id());
  // Via del Lavoro
  tl3.setDelay(std::make_pair(88, 50));  // 40, 70
  tl3.setCapacity(1);
  tl3.addStreetPriority(s23.id());
  // Viali
  tl4.setDelay(std::make_pair(81, 50));  // 38, 106 = 144
  tl4.setCapacity(1);
  tl4.addStreetPriority(s34.id());

  Graph graph;
  graph.addNode(std::make_unique<TrafficLight>(tl1));
  graph.addNode(std::make_unique<TrafficLight>(tl2));
  graph.addNode(std::make_unique<TrafficLight>(tl3));
  graph.addNode(std::make_unique<TrafficLight>(tl4));
  graph.addStreets(s01, s12, s23, s34);
  graph.buildAdj();
  graph.makeSpireStreet(19);

  std::cout << "Intersections: " << graph.nodeSet().size() << '\n';
  std::cout << "Streets: " << graph.streetSet().size() << '\n';

  // Create the dynamics
  Dynamics dynamics{graph};
  dynamics.setSeed(69);
  dynamics.setMinSpeedRateo(0.95);
  dynamics.setSpeedFluctuationSTD(0.2);
  Itinerary itinerary{0, 4};
  dynamics.addItinerary(itinerary);
  dynamics.updatePaths();

  auto& spire = dynamic_cast<SpireStreet&>(*dynamics.graph().streetSet().at(19));

  // lauch progress bar
  std::thread t([MAX_TIME]() {
    while (progress < MAX_TIME) {
      printLoadingBar(progress, MAX_TIME);
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  });
  // Evolution
  auto it = vehiclesToInsert.begin();
  std::ofstream ofs{"./stalingrado_output.csv"};
  // print two columns, time and vehicles
  ofs << "time;vehicle_flux;" << '\n';
  while (progress < MAX_TIME) {
    if (progress % 60 == 0) {
      if (progress != 0) {
        ++it;
      }
      if (progress % 300 == 0) {
        ofs << progress << ";" << spire.outputCounts(true) << std::endl;
      }
      dynamics.addAgents(0, *it / 2, 0);
    }
    dynamics.evolve(false);
    ++progress;
  }
  t.join();

  return 0;
}