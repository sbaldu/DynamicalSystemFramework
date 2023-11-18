#include <cstdint>

#include "Graph.hpp"
#include "Node.hpp"
#include "Street.hpp"
#include "SparseMatrix.hpp"

#include "doctest.h"

using Graph = dsm::Graph<uint16_t, uint16_t>;
using SparseMatrix = dsm::SparseMatrix<uint16_t, bool>;
using Street = dsm::Street<uint16_t, uint16_t>;

TEST_CASE("Graph") {
  SUBCASE("Constructor_1") {
    Street street{1, std::make_pair(0, 1)};
    Graph graph{};
    graph.addStreet(street);
    graph.buildAdj();
    CHECK(graph.streetSet().size() == 1);
    CHECK_EQ(graph.nodeSet().size(), 2);
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

  SUBCASE("Construction with addStreet") {
    Street s1(1, std::make_pair(0, 1));
    Street s2(2, std::make_pair(1, 2));
    Street s3(3, std::make_pair(0, 2));
    Street s4(4, std::make_pair(0, 3));
    Street s5(5, std::make_pair(2, 3));
    Graph graph;
    graph.addStreet(s1);
    graph.addStreet(s2);
    graph.addStreet(s3);
    graph.addStreet(s4);
    graph.addStreet(s5);
    graph.buildAdj();

    CHECK_EQ(graph.streetSet().size(), 5);
    CHECK_EQ(graph.nodeSet().size(), 4);
    CHECK_EQ(graph.adjMatrix()->size(), 5);
    CHECK(graph.adjMatrix()->contains(0, 1));
    CHECK(graph.adjMatrix()->contains(1, 2));
    CHECK(graph.adjMatrix()->contains(0, 2));
    CHECK_FALSE(graph.adjMatrix()->contains(1, 3));
  }

  SUBCASE("importAdj - dsm") {
    // This tests the importAdj function over .dsm files
    // GIVEN: a graph
    // WHEN: we import a .dsm file
    // THEN: the graph's adjacency matrix is the same as the one in the file
    Graph graph{};
    graph.importAdj("./data/matrix.dsm");
    CHECK_EQ(graph.adjMatrix()->max_size(), 9);
    CHECK_EQ(graph.adjMatrix()->getRowDim(), 3);
    CHECK_EQ(graph.adjMatrix()->getColDim(), 3);
    CHECK(graph.adjMatrix()->operator()(8));
    CHECK(graph.adjMatrix()->operator()(6));
    CHECK(graph.adjMatrix()->operator()(3));
    CHECK(graph.adjMatrix()->operator()(1));
    CHECK(graph.nodeSet().size() == 3);
    CHECK(graph.streetSet().size() == 4);
  }
  SUBCASE("importAdj - EXCEPTIONS") {
    // This tests the importAdj throws an exception when the file has not the correct format or is not found
    // GIVEN: a graph
    // WHEN: we import a file with a wrong format
    // THEN: an exception is thrown
    Graph graph{};
    CHECK_THROWS(graph.importAdj("./data/matrix.nogood"));
    CHECK_THROWS(graph.importAdj("./data/not_found.dsm"));
  }
}
