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
  graph.importMatrix("./data/matrix.dsm");
  SUBCASE("Constructor") {
    /// GIVEN: a graph
    /// WHEN: we create a dynamics object
    /// THEN: the graph is the same as the one in the dynamics object
    Dynamics dynamics(graph);
    CHECK_EQ(dynamics.graph().nodeSet().size(), 3);
    CHECK_EQ(dynamics.graph().streetSet().size(), 4);
    CHECK_EQ(dynamics.meanSpeed().mean, 0.);
    CHECK_EQ(dynamics.meanSpeed().error, 0.);
    // CHECK_EQ(dynamics.meanSpeed(2).first, 0.);
    // CHECK_EQ(dynamics.meanSpeed(2).second, 0.);
    CHECK_EQ(dynamics.meanDensity().mean, 0.);
    CHECK_EQ(dynamics.meanDensity().error, 0.);
    // CHECK_EQ(dynamics.meanDensity(3).first, 0.);
    // CHECK_EQ(dynamics.meanDensity(3).second, 0.);
    CHECK_EQ(dynamics.meanFlow().mean, 0.);
    CHECK_EQ(dynamics.meanFlow().error, 0.);
    CHECK_EQ(dynamics.meanTravelTime().mean, 0.);
    CHECK_EQ(dynamics.meanTravelTime().error, 0.);
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
    CHECK(dynamics.itineraries().at(0)->path()(0, 1));
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
    CHECK_EQ(dynamics.itineraries().size(), 1);
    CHECK_EQ(dynamics.itineraries().at(0)->path().size(), 4);
    CHECK_EQ(dynamics.itineraries().at(0)->path().getRowDim(), 4);
    CHECK_EQ(dynamics.itineraries().at(0)->path().getColDim(), 4);
    CHECK(dynamics.itineraries().at(0)->path()(0, 1));
    CHECK(dynamics.itineraries().at(0)->path()(1, 2));
    CHECK(dynamics.itineraries().at(0)->path()(0, 3));
    CHECK(dynamics.itineraries().at(0)->path()(3, 2));
  }
  SUBCASE("addAgents") {
    /// GIVEN: a dynamics object
    /// WHEN: we add agents
    /// THEN: the agents are added
    Dynamics dynamics{graph};
    Itineary itinerary{0, 0, 2};
    dynamics.addItinerary(itinerary);
    CHECK_THROWS(dynamics.addAgents(1));
    dynamics.addAgents(0);
    CHECK_EQ(dynamics.agents().size(), 1);
    dynamics.addAgents(0, 68);
    CHECK_EQ(dynamics.agents().size(), 69);  // nice
  }
}
