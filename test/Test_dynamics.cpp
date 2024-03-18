#include <cstdint>

#include "Dynamics.hpp"
#include "Graph.hpp"
#include "Node.hpp"
#include "Street.hpp"
#include "SparseMatrix.hpp"

#include "doctest.h"

using Dynamics = dsm::FirstOrderDynamics<uint16_t, uint16_t, uint16_t>;
using Graph = dsm::Graph<uint16_t, uint16_t>;
using SparseMatrix = dsm::SparseMatrix<uint16_t, bool>;
using Street = dsm::Street<uint16_t, uint16_t>;
using Agent = dsm::Agent<uint16_t, uint16_t, uint16_t>;
using Itinerary = dsm::Itinerary<uint16_t>;
using TrafficLight = dsm::TrafficLight<uint16_t, uint16_t, uint16_t>;

TEST_CASE("Dynamics") {
  SUBCASE("Constructor") {
    GIVEN("A graph object") {
      auto graph = Graph{};
      graph.importMatrix("./data/matrix.dsm");
      WHEN("A dynamics object is created") {
        Dynamics dynamics(graph);
        THEN("The node and the street sets are the same") {
          CHECK_EQ(dynamics.graph().nodeSet().size(), 3);
          CHECK_EQ(dynamics.graph().streetSet().size(), 4);
        }
        THEN("The mean speed, density, flow and travel time are 0") {
          CHECK_EQ(dynamics.meanSpeed().mean, 0.);
          CHECK_EQ(dynamics.meanSpeed().error, 0.);
          CHECK_EQ(dynamics.streetMeanDensity().mean, 0.);
          CHECK_EQ(dynamics.streetMeanDensity().error, 0.);
          CHECK_EQ(dynamics.streetMeanFlow().mean, 0.);
          CHECK_EQ(dynamics.streetMeanFlow().error, 0.);
          CHECK_EQ(dynamics.meanTravelTime().mean, 0.);
          CHECK_EQ(dynamics.meanTravelTime().error, 0.);
        }
      }
    }
  }
  SUBCASE("addAgentsUniformly") {
    GIVEN("A dynamics object and an itinerary") {
      auto graph = Graph{};
      graph.importMatrix("./data/matrix.dsm");
      Dynamics dynamics(graph);
      WHEN("We add agents without adding itineraries") {
        THEN("An exception is thrown") { CHECK_THROWS(dynamics.addAgentsUniformly(1)); }
      }
      Itinerary itinerary{0, 2};
      WHEN("We add a random agent") {
        dynamics.addItinerary(itinerary);
        dynamics.addAgentsUniformly(1);
        THEN(
            "The number of agents is 1 and the destination is the same as the "
            "itinerary") {
          CHECK_EQ(dynamics.agents().size(), 1);
          CHECK_EQ(dynamics.itineraries()
                       .at(dynamics.agents().at(0)->itineraryId())
                       ->destination(),
                   itinerary.destination());
        }
      }
    }
    GIVEN("A dynamics object and many itineraries") {
      auto graph = Graph{};
      graph.importMatrix("./data/matrix.dsm");
      Dynamics dynamics(graph);
      dynamics.setSeed(69);
      Itinerary Itinerary1{0, 2}, Itinerary2{1, 1};
      dynamics.addItinerary(Itinerary1);
      dynamics.addItinerary(Itinerary2);
      WHEN("We add many agents") {
        dynamics.addAgentsUniformly(3);
        THEN(
            "The number of agents is 3, the destination and the street is the same as "
            "the itinerary") {
          CHECK_EQ(dynamics.agents().size(), 3);
          CHECK_EQ(dynamics.itineraries()
                       .at(dynamics.agents().at(0)->itineraryId())
                       ->destination(),
                   Itinerary2.destination());
          CHECK(dynamics.agents().at(0)->streetId().has_value());
          CHECK_EQ(dynamics.agents().at(0)->streetId().value(), 6);
          CHECK_EQ(dynamics.itineraries()
                       .at(dynamics.agents().at(1)->itineraryId())
                       ->destination(),
                   Itinerary2.destination());
          CHECK(dynamics.agents().at(1)->streetId().has_value());
          CHECK_EQ(dynamics.agents().at(1)->streetId().value(), 1);
          CHECK_EQ(dynamics.itineraries()
                       .at(dynamics.agents().at(2)->itineraryId())
                       ->destination(),
                   Itinerary1.destination());
          CHECK(dynamics.agents().at(2)->streetId().has_value());
          CHECK_EQ(dynamics.agents().at(2)->streetId().value(), 8);
        }
      }
    }
  }
  SUBCASE("addAgents") {
    GIVEN("A dynamics object and one itinerary") {
      auto graph = Graph{};
      graph.importMatrix("./data/matrix.dsm");
      Dynamics dynamics{graph};
      Itinerary itinerary{0, 2};
      dynamics.addItinerary(itinerary);
      WHEN("We add an agent with itinerary 1") {
        THEN("An exception is thrown") { CHECK_THROWS(dynamics.addAgents(1)); }
      }
      WHEN("We add and agent with itinerary 0") {
        dynamics.addAgents(0);
        THEN(
            "The number of agents is 1 and the destination is the same as the "
            "itinerary") {
          CHECK_EQ(dynamics.agents().size(), 1);
          CHECK_EQ(dynamics.itineraries()
                       .at(dynamics.agents().at(0)->itineraryId())
                       ->destination(),
                   itinerary.destination());
        }
      }
      WHEN("We add 69 agents with itinerary 0") {
        dynamics.addAgents(0, 69);
        THEN("The number of agents is 69") { CHECK_EQ(dynamics.agents().size(), 69); }
      }
    }
  }
  SUBCASE("Update paths") {
    GIVEN("A dynamics object, many streets and an itinerary") {
      Street s1{0, 1, 2., std::make_pair(0, 1)};
      Street s2{1, 1, 5., std::make_pair(1, 2)};
      Street s3{2, 1, 10., std::make_pair(0, 2)};
      Graph graph2;
      graph2.addStreets(s1, s2, s3);
      graph2.buildAdj();
      Dynamics dynamics{graph2};
      Itinerary itinerary{0, 2};
      WHEN("We add an itinerary and update the paths") {
        dynamics.addItinerary(itinerary);
        dynamics.updatePaths();
        THEN(
            "The number of itineraries is 1 and the path is updated and correctly "
            "formed") {
          CHECK_EQ(dynamics.itineraries().size(), 1);
          CHECK(dynamics.itineraries().at(0)->path()(0, 1));
          CHECK(dynamics.itineraries().at(0)->path()(1, 2));
          CHECK_FALSE(dynamics.itineraries().at(0)->path()(0, 2));
          for (auto const& it : dynamics.itineraries()) {
            auto const& path = it.second->path();
            for (uint16_t i{0}; i < path.getRowDim(); ++i) {
              if (i == it.second->destination()) {
                CHECK_FALSE(path.getRow(i).size());
              } else {
                CHECK(path.getRow(i).size());
              }
            }
          }
        }
      }
    }
    GIVEN("A dynamics objects, many streets and many itinearies with same destination") {
      Graph graph2{};
      graph2.importMatrix("./data/matrix.dat");
      Itinerary it1{0, 118};
      Itinerary it2{1, 118};
      Itinerary it3{2, 118};
      Itinerary it4{3, 118};
      Dynamics dynamics{graph2};
      dynamics.addItinerary(it1);
      dynamics.addItinerary(it2);
      dynamics.addItinerary(it3);
      dynamics.addItinerary(it4);
      dynamics.updatePaths();
      for (auto const& it : dynamics.itineraries()) {
        auto const& path = it.second->path();
        for (uint16_t i{0}; i < path.getRowDim(); ++i) {
          if (i == it.second->destination()) {
            CHECK_FALSE(path.getRow(i).size());
          } else {
            CHECK(path.getRow(i).size());
          }
        }
      }
    }
    GIVEN("A dynamics objects, many streets and an itinerary with bifurcations") {
      Street s1{0, 1, 5., std::make_pair(0, 1)};
      Street s2{1, 1, 5., std::make_pair(1, 2)};
      Street s3{2, 1, 5., std::make_pair(0, 3)};
      Street s4{3, 1, 5., std::make_pair(3, 2)};
      Graph graph;
      graph.addStreets(s1, s2, s3, s4);
      graph.buildAdj();
      Dynamics dynamics{graph};
      Itinerary itinerary{0, 2};
      dynamics.addItinerary(itinerary);
      WHEN("We update the paths") {
        dynamics.updatePaths();
        THEN("The path is updated and correctly formed") {
          CHECK_EQ(dynamics.itineraries().size(), 1);
          CHECK_EQ(dynamics.itineraries().at(0)->path().size(), 4);
          CHECK_EQ(dynamics.itineraries().at(0)->path().getRowDim(), 4);
          CHECK_EQ(dynamics.itineraries().at(0)->path().getColDim(), 4);
          CHECK(dynamics.itineraries().at(0)->path()(0, 1));
          CHECK(dynamics.itineraries().at(0)->path()(1, 2));
          CHECK(dynamics.itineraries().at(0)->path()(0, 3));
          CHECK(dynamics.itineraries().at(0)->path()(3, 2));
          for (auto const& it : dynamics.itineraries()) {
            auto const& path = it.second->path();
            for (uint16_t i{0}; i < path.getRowDim(); ++i) {
              if (i == it.second->destination()) {
                CHECK_FALSE(path.getRow(i).size());
              } else {
                CHECK(path.getRow(i).size());
              }
            }
          }
        }
      }
    }
  }
  SUBCASE("Evolve") {
    GIVEN("A dynamics object and an itinerary") {
      Street s1{0, 1, 2., std::make_pair(0, 1)};
      Street s2{1, 1, 5., std::make_pair(1, 2)};
      Street s3{2, 1, 10., std::make_pair(0, 2)};
      Graph graph;
      graph.addStreets(s1, s2, s3);
      graph.buildAdj();
      Dynamics dynamics{graph};
      dynamics.setSeed(69);
      Itinerary itinerary{0, 2};
      dynamics.addItinerary(itinerary);
      dynamics.updatePaths();
      WHEN("We add an agent randomly and evolve the dynamics") {
        dynamics.addAgent(Agent(0, 0, 0));
        dynamics.evolve(false);
        dynamics.evolve(false);
        THEN("The agent evolves") {
          CHECK_EQ(dynamics.agents().at(0)->time(), 2);
          CHECK_EQ(dynamics.agents().at(0)->delay(), 0);
          CHECK(dynamics.agents().at(0)->streetId().has_value());
          CHECK_EQ(dynamics.agents().at(0)->streetId().value(), 1);
          CHECK_EQ(dynamics.agents().at(0)->speed(), 13.8888888889);
        }
        dynamics.evolve(false);
        THEN("The agent evolves again, changing street") {
          CHECK_EQ(dynamics.agents().at(0)->time(), 3);
          CHECK_EQ(dynamics.agents().at(0)->delay(), 0);
          CHECK_EQ(dynamics.agents().at(0)->streetId().value(), 5);
          CHECK_EQ(dynamics.agents().at(0)->speed(), 13.8888888889);
        }
        dynamics.evolve(false);
        THEN("And again, reaching the destination") {
          CHECK_EQ(dynamics.agents().size(), 0);
        }
      }
    }
    GIVEN("A dynamics object, an itinerary and an agent") {
      Street s1{0, 1, 13.8888888889, std::make_pair(0, 1)};
      Street s2{1, 1, 13.8888888889, std::make_pair(1, 0)};
      Graph graph2;
      graph2.addStreets(s1, s2);
      graph2.buildAdj();
      Dynamics dynamics{graph2};
      dynamics.setSeed(69);
      Itinerary itinerary{0, 1};
      dynamics.addItinerary(itinerary);
      dynamics.updatePaths();
      dynamics.addAgent(Agent(0, 0, 0));
      WHEN("We evolve the dynamics") {
        dynamics.evolve(false);
        dynamics.evolve(false);
        THEN("The agent evolves") {
          CHECK_EQ(dynamics.agents().at(0)->time(), 2);
          CHECK_EQ(dynamics.agents().at(0)->delay(), 0);
          CHECK_EQ(dynamics.agents().at(0)->streetId().value(), 1);
          CHECK_EQ(dynamics.agents().at(0)->speed(), 13.8888888889);
          CHECK_EQ(dynamics.agents().at(0)->distance(), 13.8888888889);
        }
        dynamics.evolve(false);
        THEN("The agent reaches the destination") {
          CHECK_EQ(dynamics.agents().size(), 0);
        }
      }
    }
    GIVEN("A dynamics object, an itinerary and an agent") {
      Street s1{0, 1, 13.8888888889, std::make_pair(0, 1)};
      Street s2{1, 1, 13.8888888889, std::make_pair(1, 0)};
      Graph graph2;
      graph2.addStreets(s1, s2);
      graph2.buildAdj();
      Dynamics dynamics{graph2};
      dynamics.setSeed(69);
      Itinerary itinerary{0, 1};
      dynamics.addItinerary(itinerary);
      dynamics.updatePaths();
      dynamics.addAgent(Agent(0, 0, 0));
      WHEN("We evolve the dynamics with reinsertion") {
        dynamics.evolve(true);
        dynamics.evolve(true);
        THEN("The agent has correct values") {
          CHECK_EQ(dynamics.agents().at(0)->time(), 2);
          CHECK_EQ(dynamics.agents().at(0)->delay(), 0);
          CHECK_EQ(dynamics.agents().at(0)->streetId().value(), 1);
          CHECK_EQ(dynamics.agents().at(0)->speed(), 13.8888888889);
          CHECK_EQ(dynamics.agents().at(0)->distance(), 13.8888888889);
        }
        dynamics.evolve(true);
        THEN("The agent is reinserted") {
          CHECK_EQ(dynamics.agents().size(), 1);
          CHECK_EQ(dynamics.agents().at(0)->time(), 1);
          CHECK_EQ(dynamics.agents().at(0)->delay(), 0);
          CHECK_FALSE(dynamics.agents().at(0)->streetId().has_value());
          CHECK_EQ(dynamics.agents().at(0)->speed(), 0.);
        }
      }
    }
  }
  SUBCASE("TrafficLights") {
    GIVEN("A dynamics object, a network with traffic lights, an itinerary and an agent") {
      TrafficLight tl{1};
      tl.setDelay(2);
      Street s1{0, 1, 30., 15., std::make_pair(0, 1)};
      Street s2{1, 1, 30., 15., std::make_pair(1, 2)};
      Street s3{2, 1, 30., 15., std::make_pair(3, 1)};
      Street s4{3, 1, 30., 15., std::make_pair(1, 4)};
      tl.addStreetPriority(0);
      tl.addStreetPriority(1);
      Graph graph2;
      graph2.addNode(std::make_unique<TrafficLight>(tl));
      graph2.addStreets(s1, s2, s3, s4);
      graph2.buildAdj();
      Dynamics dynamics{graph2};
      dynamics.setSeed(69);
      Itinerary itinerary{0, 2};
      dynamics.addItinerary(itinerary);
      dynamics.updatePaths();
      dynamics.addAgent(Agent(0, 0, 0));
      WHEN("We evolve the dynamics") {
        // dynamics.evolve(false);
        THEN(
            "The agent is ready to go through the traffic light at time 3 but the "
            "traffic light is red"
            " until time 4, so the agent waits until time 4") {
          for (uint8_t i{0}; i < 5; ++i) {
            dynamics.evolve(false);
            if (i > 0 && i < 3) {
              CHECK_EQ(dynamics.agents().at(0)->streetId().value(), 1);
            } else if (i > 2) {
              CHECK_EQ(dynamics.agents().at(0)->streetId().value(), 7);
            }
            if (i == 2) {
              CHECK_EQ(dynamics.agents().at(0)->distance(), 30.);
            }
          }
          CHECK_EQ(dynamics.agents().at(0)->distance(), 60.);
          dynamics.evolve(false);
          CHECK_EQ(dynamics.agents().size(), 0);
        }
      }
    }
  }
  SUBCASE("Travelled distance") {
    GIVEN("A dynamics with a two-streets network and an agent") {
      Street s1{0, 1, 3., std::make_pair(0, 1)};
      Street s2{1, 1, 1., std::make_pair(1, 2)};
      Graph graph2;
      graph2.addStreets(s1, s2);
      graph2.buildAdj();
      Dynamics dynamics{graph2};
      dynamics.setSeed(69);
      Itinerary itinerary{0, 2};
      dynamics.addItinerary(itinerary);
      dynamics.updatePaths();
      dynamics.addAgent(Agent(0, 0, 0));
      WHEN("We evolve the dynamics") {
        dynamics.evolve(false);
        dynamics.evolve(false);
        THEN("The agent has travelled the correct distance") {
          CHECK_EQ(dynamics.agents().at(0)->time(), 2);
          CHECK_EQ(dynamics.agents().at(0)->delay(), 0);
          CHECK_EQ(dynamics.agents().at(0)->streetId().value(), 1);
          CHECK_EQ(dynamics.agents().at(0)->speed(), 13.8888888889);
          CHECK_EQ(dynamics.agents().at(0)->distance(), 3.);
        }
      }
    }
  }
  SUBCASE("streetMeanSpeed") {
    /// GIVEN: a dynamics object
    /// WHEN: we evolve the dynamics
    /// THEN: the agent mean speed is the same as the street mean speed
    Street s1{0, 10, 20., 20., std::make_pair(0, 1)};
    Street s2{1, 10, 30., 15., std::make_pair(1, 2)};
    Street s3{2, 10, 30., 15., std::make_pair(3, 1)};
    Street s4{3, 10, 30., 15., std::make_pair(1, 4)};
    Graph graph2;
    graph2.addStreets(s1, s2, s3, s4);
    graph2.buildAdj();
    for (const auto& [nodeId, node] : graph2.nodeSet()) {
      node->setCapacity(4);
    }
    Dynamics dynamics{graph2};
    dynamics.setMinSpeedRateo(0.5);
    dynamics.setSeed(69);
    Itinerary itinerary{0, 2};
    dynamics.addItinerary(itinerary);
    dynamics.updatePaths();
    dynamics.addAgents(0, 4, 0);
    dynamics.evolve(false);
    dynamics.evolve(false);
    double meanSpeed{0.};
    for (const auto& [agentId, agent] : dynamics.agents()) {
      meanSpeed += agent->speed();
    }
    meanSpeed /= dynamics.graph().streetSet().at(1)->queue().size();
    CHECK(dynamics.streetMeanSpeed(1).has_value());
    CHECK_EQ(dynamics.streetMeanSpeed(1).value(), meanSpeed);
    CHECK_EQ(dynamics.streetMeanSpeed().mean, dynamics.meanSpeed().mean);
    CHECK_EQ(dynamics.streetMeanSpeed().error, 0.);
    // street 1 density should be 0.4 so...
    CHECK_EQ(dynamics.streetMeanSpeed(0.2, true).mean, meanSpeed);
    CHECK_EQ(dynamics.streetMeanSpeed(0.2, true).error, 0.);
    CHECK_EQ(dynamics.streetMeanSpeed(0.2, false).mean, 0.);
    CHECK_EQ(dynamics.streetMeanSpeed(0.2, false).error, 0.);
    dynamics.addAgents(0, 10, 0);
    dynamics.evolve(false);
    meanSpeed = 0.;
    for (const auto& [agentId, agent] : dynamics.agents()) {
      if (!agent->streetId().has_value())
        continue;
      if (agent->streetId().value() == 1) {
        meanSpeed += agent->speed();
      }
    }
    meanSpeed /= dynamics.graph().streetSet().at(1)->queue().size();
    CHECK_EQ(dynamics.graph().streetSet().at(1)->queue().size(), 3);
    CHECK(dynamics.streetMeanSpeed(1).has_value());
    CHECK_EQ(dynamics.streetMeanSpeed(1).value(), meanSpeed);
    dynamics.graph().streetSet().at(1)->setIsSpire(false);
    CHECK_FALSE(dynamics.streetMeanSpeed(1).has_value());
  }
}
