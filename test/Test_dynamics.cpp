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
using Itineary = dsm::Itinerary<uint16_t>;

TEST_CASE("Dynamics") {
  auto graph = Graph{};
  graph.importAdj("./data/matrix.dsm");
  SUBCASE("Constructor") {
    /// GIVEN: a graph
    /// WHEN: we create a dynamics object
    /// THEN: the graph is the same as the one in the dynamics object
    Dynamics dynamics(graph);
    CHECK_EQ(dynamics.graph().nodeSet().size(), 3);
    CHECK_EQ(dynamics.graph().streetSet().size(), 4);
    CHECK_EQ(dynamics.meanSpeed(), 0.);
    CHECK_EQ(dynamics.meanDensity(), 0.);
    CHECK_EQ(dynamics.meanFlow(), 0.);
    CHECK_EQ(dynamics.meanTravelTime(), 0.);
  }
  SUBCASE("AddRandomAgents") {
    /// GIVEN: an itineary
    /// WHEN: we add a random agent to the dynamics
    /// THEN: the agent has the same itineary
    Dynamics dynamics(graph);
    Itineary itineary{0, 0, 2};
    dynamics.addItinerary(itineary);
    dynamics.addRandomAgents(1);
    CHECK_EQ(dynamics.agents().size(), 1);
    auto agents = dynamics.agents();
    auto itineraries = dynamics.itineraries();
    CHECK_EQ(itineraries[agents[0]->itineraryId()]->source(), itineary.source());
    CHECK_EQ(itineraries[agents[0]->itineraryId()]->destination(), itineary.destination());
  }
  SUBCASE("AddRandomAgents with many itineraries") {
    /// GIVEN: a dynamics object
    /// WHEN: we add many itineraries
    /// THEN: the number of agents is the same as the number of itineraries
    Dynamics dynamics(graph);
    dynamics.setSeed(69);
    Itineary itineary{0, 0, 2}, itineary2{1, 1, 2}, itineary3{2, 0, 1};
    dynamics.addItinerary(itineary);
    dynamics.addItinerary(itineary2);
    dynamics.addItinerary(itineary3);
    dynamics.addRandomAgents(3);
    CHECK_EQ(dynamics.agents().size(), 3);
    auto agents = dynamics.agents();
    auto itineraries = dynamics.itineraries();
    CHECK_EQ(itineraries[agents[0]->itineraryId()]->source(), itineary2.source());
    CHECK_EQ(itineraries[agents[0]->itineraryId()]->destination(), itineary2.destination());
    CHECK_EQ(itineraries[agents[1]->itineraryId()]->source(), itineary.source());
    CHECK_EQ(itineraries[agents[1]->itineraryId()]->destination(), itineary.destination());
    CHECK_EQ(itineraries[agents[2]->itineraryId()]->source(), itineary3.source());
    CHECK_EQ(itineraries[agents[2]->itineraryId()]->destination(), itineary3.destination());
  }
  SUBCASE("AddRandomAgents - exceptions") {
    /// GIVEN: a dynamics object
    /// WHEN: we add a random agent with a negative number of agents
    /// THEN: an exception is thrown
    Dynamics dynamics(graph);
    CHECK_THROWS(dynamics.addRandomAgents(1));
  }
  SUBCASE("updatePaths") {
    /// GIVEN: a dynamics object
    /// WHEN: we update the paths
    /// THEN: the paths are updated
    Street s1{0, 1, 2., std::make_pair(0, 1)};
    Street s2{1, 1, 5., std::make_pair(1, 2)};
    Street s3{2, 1, 10., std::make_pair(0, 2)};
    Graph graph2;
    graph2.addStreets(s1, s2, s3);
    graph2.buildAdj();
    Dynamics dynamics{graph2};
    Itineary itinerary{0, 0, 2};
    dynamics.addItinerary(itinerary);
    dynamics.updatePaths();
    CHECK_EQ(dynamics.itineraries().size(), 1);
    auto itineraries = dynamics.itineraries();
    CHECK(itineraries[0]->path()(0, 1));
  }
  SUBCASE("updatePaths - equal length") {
    /// GIVEN: a dynamics object
    /// WHEN: we update the paths
    /// THEN: the paths are updated
    Street s1{0, 1, 5., std::make_pair(0, 1)};
    Street s2{1, 1, 5., std::make_pair(1, 2)};
    Street s3{2, 1, 5., std::make_pair(0, 3)};
    Street s4{3, 1, 5., std::make_pair(3, 2)};
    Graph graph2;
    graph2.addStreets(s1, s2, s3, s4);
    graph2.buildAdj();
    Dynamics dynamics{graph2};
    Itineary itinerary{0, 0, 2};
    dynamics.addItinerary(itinerary);
    dynamics.updatePaths();
    auto itineraries = dynamics.itineraries();
    CHECK_EQ(itineraries.size(), 1);
    CHECK_EQ(itineraries[0]->path().size(), 4);
    CHECK_EQ(itineraries[0]->path().getRowDim(), 4);
    CHECK_EQ(itineraries[0]->path().getColDim(), 4);
    CHECK(itineraries[0]->path()(0, 1));
    CHECK(itineraries[0]->path()(1, 2));
    CHECK(itineraries[0]->path()(0, 3));
    CHECK(itineraries[0]->path()(3, 2));
  }
  SUBCASE("evolve with one agent") {
    /// GIVEN: a dynamics object
    /// WHEN: we evolve the dynamics
    /// THEN: the dynamics evolves
    Street s1{0, 1, 2., std::make_pair(0, 1)};
    Street s2{1, 1, 5., std::make_pair(1, 2)};
    Street s3{2, 1, 10., std::make_pair(0, 2)};
    Graph graph2;
    graph2.addStreets(s1, s2, s3);
    graph2.buildAdj();
    Dynamics dynamics{graph2};
    dynamics.setSeed(69);
    Itineary itinerary{0, 0, 2};
    dynamics.addItinerary(itinerary);
    dynamics.addRandomAgents(1);
    dynamics.updatePaths();
    dynamics.evolve(false);
    auto agents = dynamics.agents();
    CHECK_EQ(agents[0]->time(), 1);
    CHECK_EQ(agents[0]->delay(), 0);
    CHECK_FALSE(agents[0]->streetId().has_value());
    dynamics.evolve(false);
    CHECK_EQ(agents[0]->time(), 2);
    CHECK_EQ(agents[0]->delay(), 0);
    CHECK_EQ(agents[0]->streetId().value(), 1);
    CHECK_EQ(agents[0]->speed(), 30);
  }
  SUBCASE("evolve without insertion") {
    /// GIVEN: a dynamics object
    /// WHEN: we evolve the dynamics
    /// THEN: the dynamics evolves
    Street s1{0, 1, 30., std::make_pair(0, 1)};
    Street s2{1, 1, 30., std::make_pair(1, 0)};
    Graph graph2;
    graph2.addStreets(s1, s2);
    graph2.buildAdj();
    Dynamics dynamics{graph2};
    dynamics.setSeed(69);
    Itineary itinerary{0, 0, 1};
    dynamics.addItinerary(itinerary);
    dynamics.addRandomAgents(1);
    dynamics.updatePaths();
    // dynamics.evolve(false);
    // dynamics.evolve(false);
    for (uint8_t i = 0; i < 2; ++i) {
      dynamics.evolve(false);
    }
    auto agents = dynamics.agents();
    CHECK_EQ(agents[0]->time(), 2);
    CHECK_EQ(agents[0]->delay(), 0);
    CHECK_EQ(agents[0]->streetId().value(), 1);
    CHECK_EQ(agents[0]->speed(), 30);
    dynamics.evolve(false);
    agents = dynamics.agents();
    CHECK_EQ(agents.size(), 0);
  }
  SUBCASE("evolve with reinsertion") {
    /// GIVEN: a dynamics object
    /// WHEN: we evolve the dynamics
    /// THEN: the dynamics evolves
    Street s1{0, 1, 30., std::make_pair(0, 1)};
    Street s2{1, 1, 30., std::make_pair(1, 0)};
    Graph graph2;
    graph2.addStreets(s1, s2);
    graph2.buildAdj();
    Dynamics dynamics{graph2};
    dynamics.setSeed(69);
    Itineary itinerary{0, 0, 1};
    dynamics.addItinerary(itinerary);
    dynamics.addRandomAgents(1);
    dynamics.updatePaths();
    // dynamics.evolve(false);
    // dynamics.evolve(false);
    for (uint8_t i = 0; i < 2; ++i) {
      dynamics.evolve(true);
    }
    auto agents = dynamics.agents();
    CHECK_EQ(agents[0]->time(), 2);
    CHECK_EQ(agents[0]->delay(), 0);
    CHECK_EQ(agents[0]->streetId().value(), 1);
    CHECK_EQ(agents[0]->speed(), 30);
    dynamics.evolve(true);
    agents = dynamics.agents();
    CHECK_EQ(agents.size(), 1);
    CHECK_EQ(agents[0]->time(), 1);
    CHECK_EQ(agents[0]->delay(), 0);
    CHECK_FALSE(agents[0]->streetId().has_value());
    CHECK_EQ(agents[0]->speed(), 0.);
  }
}