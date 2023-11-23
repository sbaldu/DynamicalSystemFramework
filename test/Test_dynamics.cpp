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
        CHECK(dynamics.graph().nodeSet().size() == 3);
        CHECK(dynamics.graph().streetSet().size() == 4);
        CHECK(dynamics.meanSpeed() == 0.);
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
    SUBCASE("AddRandomAgents with many itineraries") {
        /// GIVEN: a dynamics object
        /// WHEN: we add many itineraries
        /// THEN: the number of agents is the same as the number of itineraries
        Dynamics dynamics(graph);
        dynamics.setSeed(69);
        Itineary itineary{0, 2}, itineary2{1, 2}, itineary3{0, 1};
        dynamics.addItinerary(itineary);
        dynamics.addItinerary(itineary2);
        dynamics.addItinerary(itineary3);
        dynamics.addRandomAgents(3);
        CHECK(dynamics.agents().size() == 3);
        CHECK(dynamics.agents()[0]->itinerary().source() == itineary2.source());
        CHECK(dynamics.agents()[0]->itinerary().destination() == itineary2.destination());
        CHECK(dynamics.agents()[1]->itinerary().source() == itineary.source());
        CHECK(dynamics.agents()[1]->itinerary().destination() == itineary.destination());
        CHECK(dynamics.agents()[2]->itinerary().source() == itineary3.source());
        CHECK(dynamics.agents()[2]->itinerary().destination() == itineary3.destination());
    }
    SUBCASE("AddRandomAgents - exceptions") {
        /// GIVEN: a dynamics object
        /// WHEN: we add a random agent with a negative number of agents
        /// THEN: an exception is thrown
        Dynamics dynamics(graph);
        CHECK_THROWS(dynamics.addRandomAgents(1));
    }
}