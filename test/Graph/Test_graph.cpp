#include <cstdint>

#include "Graph.hpp"
#include "Node.hpp"
#include "Street.hpp"
#include "SparseMatrix.hpp"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

using Graph = dsm::Graph<uint16_t, uint16_t>;
using SparseMatrix = dsm::SparseMatrix<uint16_t, bool>;
using Street = dsm::Street<uint16_t, uint16_t>;

TEST_CASE("Graph") {
  SUBCASE("Constructor_1") {
    Street street{1, 2, 3};
    Graph graph{};
    graph.addStreet(street);
    graph.buildAdj();
    CHECK(graph.streetSet().size() == 1);
    CHECK(graph.adjMatrix()->size() == 1);
  }

  SUBCASE("Constructor_2") {
    SparseMatrix sm(4, 4);
    sm.insert(0, 1, true);
    sm.insert(1, 0, true);
    sm.insert(1, 2, true);
    sm.insert(2, 3, true);
    sm.insert(3, 2, true);
    Graph graph{sm};
    CHECK(graph.nodeSet().size() == 4);
    CHECK(graph.streetSet().size() == 5);
    CHECK(graph.adjMatrix()->size() == 5);
    CHECK(graph.adjMatrix()->contains(1, 2));
    CHECK(graph.adjMatrix()->contains(2, 3));
    CHECK(graph.adjMatrix()->contains(3, 2));
    CHECK_FALSE(graph.adjMatrix()->contains(2, 1));
  }
}
