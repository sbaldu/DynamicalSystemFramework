#include <cstdint>

#include "Graph.hpp"
#include "Node.hpp"
#include "Street.hpp"
#include "HashFunctions.hpp"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

using Graph = dmf::Graph<uint16_t, uint16_t>;
using Street = dmf::Street<uint16_t, uint16_t>;

TEST_CASE("Graph") {
    SUBCASE("Constructor") {
        Street street{1, 2, 3};
        Graph graph{};
        graph.addStreet(street);
        graph.buildAdj();
        CHECK(graph.streetSet().size() == 1);
        CHECK(graph.adjMatrix()->size() == 2);
        // CHECK(graph.adjMatrix()->contains(0, 1));
        // CHECK(graph.adjMatrix()->contains(1, 0));
    }
}