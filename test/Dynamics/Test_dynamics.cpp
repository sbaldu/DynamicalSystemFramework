#include <cstdint>

#include "Dynamics.hpp"
#include "Graph.hpp"
#include "Node.hpp"
#include "Street.hpp"
#include "SparseMatrix.hpp"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
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
        CHECK(dynamics.graph().nodeSet().size() == 3);
        CHECK(dynamics.graph().streetSet().size() == 4);
    }
    SUBCASE("AddRandomAgents") {
        /// GIVEN: an itineary
        /// WHEN: we add a random agent to the dynamics
        /// THEN: the agent has the same itineary
        Dynamics dynamics(graph);
        Itineary itineary{0, 2};
        dynamics.addItinerary(itineary);
        dynamics.addRandomAgents(1);
        CHECK(dynamics.agents().size() == 1);
        CHECK(dynamics.agents()[0]->itinerary().source() == itineary.source());
        CHECK(dynamics.agents()[0]->itinerary().destination() == itineary.destination());
    }
    SUBCASE("AddRandomAgents - exceptions") {
        /// GIVEN: a dynamics object
        /// WHEN: we add a random agent with a negative number of agents
        /// THEN: an exception is thrown
        Dynamics dynamics(graph);
        CHECK_THROWS(dynamics.addRandomAgents(1));
    }
}