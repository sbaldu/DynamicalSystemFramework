
#include <cassert>
#include <cstdint>

#include "Graph.hpp"
#include "Node.hpp"
#include "Street.hpp"
#include "SparseMatrix.hpp"

#include "doctest.h"

using Graph = dsm::Graph<uint, uint>;
using SparseMatrix = dsm::SparseMatrix<uint, bool>;
using Street = dsm::Street<uint, uint>;
using Path = std::vector<uint>;

template <typename T1, typename T2>
bool checkPath(const std::vector<T1>& path1, const std::vector<T2>& path2) {
  const size_t length{path1.size()};
  assert(length == path2.size());

  bool equal{true};
  for (size_t i{}; i < length; ++i) {
    if (path1[i] != path2[i]) {
      equal = false;
    }
  }

  return equal;
}

TEST_CASE("Graph") {
  SUBCASE("Constructor_1") {
    Street street{1, std::make_pair(0, 1)};
    Graph graph{};
    graph.addStreet(street);
    graph.buildAdj();
    CHECK(graph.streetSet().size() == 1);
    CHECK_EQ(graph.nodeSet().size(), 2);
    CHECK(graph.adjMatrix().size() == 1);
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
    CHECK(graph.adjMatrix().size() == 5);
    CHECK(graph.adjMatrix().contains(1, 2));
    CHECK(graph.adjMatrix().contains(2, 3));
    CHECK(graph.adjMatrix().contains(3, 2));
    CHECK_FALSE(graph.adjMatrix().contains(2, 1));
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
    CHECK_EQ(graph.adjMatrix().size(), 5);
    CHECK(graph.adjMatrix().contains(0, 1));
    CHECK(graph.adjMatrix().contains(1, 2));
    CHECK(graph.adjMatrix().contains(0, 2));
    CHECK_FALSE(graph.adjMatrix().contains(1, 3));
  }

  SUBCASE("Construction with addStreets") {
    Street s1(1, std::make_pair(0, 1));
    Street s2(2, std::make_pair(1, 2));
    Street s3(3, std::make_pair(0, 2));
    Street s4(4, std::make_pair(0, 3));
    Street s5(5, std::make_pair(2, 3));
    Graph graph;
    graph.addStreets(s1, s2, s3, s4, s5);
    graph.buildAdj();

    CHECK_EQ(graph.streetSet().size(), 5);
    CHECK_EQ(graph.nodeSet().size(), 4);
    CHECK_EQ(graph.adjMatrix().size(), 5);
    CHECK(graph.adjMatrix().contains(0, 1));
    CHECK(graph.adjMatrix().contains(1, 2));
    CHECK(graph.adjMatrix().contains(0, 2));
    CHECK_FALSE(graph.adjMatrix().contains(1, 3));
  }

  SUBCASE("importMatrix - dsm") {
    // This tests the importMatrix function over .dsm files
    // GIVEN: a graph
    // WHEN: we import a .dsm file
    // THEN: the graph's adjacency matrix is the same as the one in the file
    Graph graph{};
    graph.importMatrix("./data/matrix.dsm");
    CHECK_EQ(graph.adjMatrix().max_size(), 9);
    CHECK_EQ(graph.adjMatrix().getRowDim(), 3);
    CHECK_EQ(graph.adjMatrix().getColDim(), 3);
    CHECK(graph.adjMatrix().operator()(8));
    CHECK(graph.adjMatrix().operator()(6));
    CHECK(graph.adjMatrix().operator()(3));
    CHECK(graph.adjMatrix().operator()(1));
    CHECK(graph.nodeSet().size() == 3);
    CHECK(graph.streetSet().size() == 4);
    graph.exportMatrix("./data/temp.dsm", false);
    Graph graph2{};
    graph2.importMatrix("./data/temp.dsm");
    CHECK_EQ(graph2.adjMatrix().max_size(), 9);
    CHECK_EQ(graph2.adjMatrix().getRowDim(), 3);
    CHECK_EQ(graph2.adjMatrix().getColDim(), 3);
    CHECK(graph2.adjMatrix().operator()(8));
    CHECK(graph2.adjMatrix().operator()(6));
    CHECK(graph2.adjMatrix().operator()(3));
    CHECK(graph2.adjMatrix().operator()(1));
    CHECK(graph2.nodeSet().size() == 3);
    CHECK(graph2.streetSet().size() == 4);
  }
  SUBCASE("importCoordinates") {
    Graph graph{};
    CHECK_THROWS(graph.importCoordinates("./data/coords.txt"));
    graph.importMatrix("./data/matrix.dsm");
    graph.importCoordinates("./data/coords.dsm");
    const auto& nodes = graph.nodeSet();
    CHECK_EQ(nodes.at(0)->coords(), std::make_pair(0., 0.));
    CHECK_EQ(nodes.at(1)->coords(), std::make_pair(1., 0.));
    CHECK_EQ(nodes.at(2)->coords(), std::make_pair(2., 0.));
  }
  SUBCASE("importMatrix - raw matrix") {
    Graph graph{};
    graph.importMatrix("./data/rawMatrix.txt", false);
    CHECK_EQ(graph.adjMatrix().max_size(), 9);
    CHECK_EQ(graph.adjMatrix().getRowDim(), 3);
    CHECK_EQ(graph.adjMatrix().getColDim(), 3);
    CHECK(graph.adjMatrix().operator()(0, 1));
    CHECK(graph.adjMatrix().operator()(1, 0));
    CHECK(graph.adjMatrix().operator()(1, 2));
    CHECK(graph.adjMatrix().operator()(2, 1));
    CHECK(graph.nodeSet().size() == 3);
    CHECK(graph.streetSet().size() == 4);
    CHECK_EQ(graph.streetSet()[1]->length(), 500);
    CHECK_EQ(graph.streetSet()[3]->length(), 200);
    CHECK_EQ(graph.streetSet()[5]->length(), 1);
    CHECK_EQ(graph.streetSet()[7]->length(), 3);
  }
  SUBCASE("importMatrix - EXCEPTIONS") {
    // This tests the importMatrix throws an exception when the file has not the correct format or is not found
    // GIVEN: a graph
    // WHEN: we import a file with a wrong format
    // THEN: an exception is thrown
    Graph graph{};
    CHECK_THROWS(graph.importMatrix("./data/matrix.nogood"));
    CHECK_THROWS(graph.importMatrix("./data/not_found.dsm"));
  }
  SUBCASE("importOSMNodes and importOSMEdges") {
    Graph graph{};
    graph.importOSMNodes("./data/nodes.csv");
    CHECK_EQ(graph.nodeSet().size(), 25);
    graph.importOSMEdges("./data/edges.csv");
    CHECK_EQ(graph.streetSet().size(), 60);
    graph.buildAdj();
    CHECK_EQ(graph.adjMatrix().size(), 60);
  }
  SUBCASE("street") {
    /// GIVEN: a graph
    /// WHEN: we add a street
    /// THEN: the street is added
    Graph graph{};
    Street street{1, 1, 1., std::make_pair(0, 1)};
    graph.addStreet(street);
    auto result = graph.street(0, 1);
    CHECK(result);
    const auto& street2 = *result;
    CHECK_EQ(street2->id(), 1);
    CHECK_EQ(street2->length(), 1.);
    CHECK_EQ(street2->capacity(), 1);
    CHECK_FALSE(graph.street(1, 0));
  }
  SUBCASE("make trafficlight") {
    GIVEN("A graph object with two nodes and one street") {
      Graph graph{};
      graph.addStreet(Street{1, 1, 1., std::make_pair(0, 1)});
      graph.buildAdj();
      WHEN("We make node 0 a traffic light") {
        graph.makeTrafficLight<uint8_t>(0);
        THEN("The node 0 is a traffic light") {
          CHECK(graph.nodeSet().at(0)->isTrafficLight());
        }
      }
    }
  }
  SUBCASE("make roundabout") {
    GIVEN("A graph object with two nodes and one street") {
      Graph graph{};
      graph.addStreet(Street{1, 1, 1., std::make_pair(0, 1)});
      graph.buildAdj();
      WHEN("We make node 0 a roundabout") {
        graph.makeRoundabout(0);
        THEN("The node 0 is a roundabout") {
          CHECK(graph.nodeSet().at(0)->isRoundabout());
        }
      }
    }
  }
  SUBCASE("make spire street") {
    GIVEN("A graph object with two nodes and one street") {
      Graph graph{};
      graph.addStreet(Street{0, 1, 1., std::make_pair(0, 1)});
      graph.buildAdj();
      WHEN("We make the street a spire street") {
        graph.makeSpireStreet(1);
        THEN("The street is a spire street") {
          CHECK(graph.streetSet().at(1)->isSpire());
        }
      }
    }
  
  }
}

TEST_CASE("Dijkstra") {
  SUBCASE("Case 1") {
    Street s1{0, 5, 3., std::make_pair(0, 1)};
    Street s2{1, 5, 2., std::make_pair(1, 2)};
    Street s3{2, 5, 4., std::make_pair(2, 3)};
    Street s4{3, 5, 5., std::make_pair(3, 0)};
    Street s5{4, 5, 6., std::make_pair(0, 2)};
    Graph graph{};
    graph.addStreets(s1, s2, s3, s4, s5);
    graph.buildAdj();
    auto result = graph.shortestPath(0, 1);
    Path correctPath{0, 1};
    CHECK(result.has_value());
    CHECK_EQ(result.value().path().size(), 2);
    CHECK(checkPath(result.value().path(), correctPath));
    CHECK_EQ(result.value().distance(), 3.);
    result = graph.shortestPath(0, 2);
    correctPath = Path{0, 1, 2};
    CHECK(result.has_value());
    CHECK_EQ(result.value().path().size(), 3);
    CHECK(checkPath(result.value().path(), correctPath));
    CHECK_EQ(result.value().distance(), 5.);
  }

  SUBCASE("Case 2") {
    Street s1(0, 5, 1., std::make_pair(0, 1));
    Street s2(1, 5, 1., std::make_pair(1, 2));
    Street s3(2, 5, 6., std::make_pair(0, 2));
    Graph graph{};
    graph.addStreets(s1, s2, s3);
    graph.buildAdj();
    auto result = graph.shortestPath(0, 2);
    Path correctPath{0, 1, 2};
    CHECK(result.has_value());
    CHECK_EQ(result.value().path().size(), 3);
    CHECK(checkPath(result.value().path(), correctPath));
    CHECK_EQ(result.value().distance(), 2.);
  }

  SUBCASE("Case 3") {
    Street s1(0, 5, 5., std::make_pair(0, 1));
    Street s2(1, 5, 4., std::make_pair(1, 2));
    Street s3(2, 5, 6., std::make_pair(0, 2));
    Graph graph{};
    graph.addStreets(s1, s2, s3);
    graph.buildAdj();
    auto result = graph.shortestPath(0, 2);
    Path correctPath{0, 2};
    CHECK(result.has_value());
    CHECK_EQ(result.value().path().size(), 2);
    CHECK(checkPath(result.value().path(), correctPath));
    CHECK_EQ(result.value().distance(), 6.);
  }

  SUBCASE("Case 4") {
    Street s1(0, 5, 3., std::make_pair(0, 1));
    Street s2(1, 5, 1., std::make_pair(0, 2));
    Street s3(2, 5, 7., std::make_pair(1, 2));
    Street s4(3, 5, 2., std::make_pair(2, 3));
    Street s5(4, 5, 1., std::make_pair(1, 4));
    Street s6(5, 5, 5., std::make_pair(1, 3));
    Street s7(6, 5, 7., std::make_pair(3, 4));
    Street s8(7, 5, 3., std::make_pair(1, 0));
    Street s9(8, 5, 1., std::make_pair(2, 0));
    Street s10(9, 5, 7., std::make_pair(2, 1));
    Street s11(10, 5, 2., std::make_pair(3, 2));
    Street s12(11, 5, 1., std::make_pair(4, 1));
    Street s13(12, 5, 5., std::make_pair(3, 1));
    Street s14(13, 5, 7., std::make_pair(4, 3));
    Graph graph{};
    graph.addStreets(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14);
    graph.buildAdj();
    auto result = graph.shortestPath(2, 4);
    Path correctPath{2, 0, 1, 4};
    CHECK(result.has_value());
    CHECK_EQ(result.value().path().size(), 4);
    CHECK(checkPath(result.value().path(), correctPath));
    CHECK_EQ(result.value().distance(), 5.);
    result = graph.shortestPath(2, 0);
    correctPath = Path{2, 0};
    CHECK(result.has_value());
    CHECK_EQ(result.value().path().size(), 2);
    CHECK(checkPath(result.value().path(), correctPath));
    CHECK_EQ(result.value().distance(), 1.);
    result = graph.shortestPath(2, 1);
    correctPath = Path{2, 0, 1};
    CHECK(result.has_value());
    CHECK_EQ(result.value().path().size(), 3);
    CHECK(checkPath(result.value().path(), correctPath));
    CHECK_EQ(result.value().distance(), 4.);
    result = graph.shortestPath(2, 3);
    correctPath = Path{2, 3};
    CHECK(result.has_value());
    CHECK_EQ(result.value().path().size(), 2);
    CHECK(checkPath(result.value().path(), correctPath));
    CHECK_EQ(result.value().distance(), 2.);
  }

  SUBCASE("Case 5") {
    Street s1(0, 5, 2., std::make_pair(0, 1));
    Street s2(1, 5, 6., std::make_pair(0, 2));
    Street s3(2, 5, 5., std::make_pair(1, 3));
    Street s4(3, 5, 8., std::make_pair(2, 3));
    Street s5(4, 5, 15., std::make_pair(3, 5));
    Street s6(5, 5, 10., std::make_pair(3, 4));
    Street s7(6, 5, 6., std::make_pair(4, 5));
    Street s8(7, 5, 2., std::make_pair(4, 6));
    Street s9(8, 5, 6., std::make_pair(5, 6));
    Street s10(9, 5, 2., std::make_pair(1, 0));
    Street s11(10, 5, 6., std::make_pair(2, 0));
    Street s12(11, 5, 5., std::make_pair(3, 1));
    Street s13(12, 5, 8., std::make_pair(3, 2));
    Street s14(13, 5, 15., std::make_pair(5, 3));
    Street s15(14, 5, 10., std::make_pair(4, 3));
    Street s16(15, 5, 6., std::make_pair(5, 4));
    Street s17(16, 5, 2., std::make_pair(6, 4));
    Street s18(17, 5, 6., std::make_pair(6, 5));
    Graph graph{};
    graph.addStreets(
        s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18);
    graph.buildAdj();
    auto result = graph.shortestPath(0, 1);
    Path correctPath{0, 1};
    CHECK(result.has_value());
    CHECK_EQ(result.value().path().size(), 2);
    CHECK(checkPath(result.value().path(), correctPath));
    CHECK_EQ(result.value().distance(), 2.);

    result = graph.shortestPath(0, 2);
    correctPath = Path{0, 2};
    CHECK(result.has_value());
    CHECK_EQ(result.value().path().size(), 2);
    CHECK(checkPath(result.value().path(), correctPath));
    CHECK_EQ(result.value().distance(), 6.);

    result = graph.shortestPath(0, 3);
    correctPath = Path{0, 1, 3};
    CHECK(result.has_value());
    CHECK_EQ(result.value().path().size(), 3);
    CHECK(checkPath(result.value().path(), correctPath));
    CHECK_EQ(result.value().distance(), 7.);

    result = graph.shortestPath(0, 4);
    correctPath = Path{0, 1, 3, 4};
    CHECK(result.has_value());
    CHECK_EQ(result.value().path().size(), 4);
    CHECK(checkPath(result.value().path(), correctPath));
    CHECK_EQ(result.value().distance(), 17.);

    result = graph.shortestPath(0, 5);
    correctPath = Path{0, 1, 3, 5};
    CHECK(result.has_value());
    CHECK_EQ(result.value().path().size(), 4);
    CHECK(checkPath(result.value().path(), correctPath));
    CHECK_EQ(result.value().distance(), 22.);

    result = graph.shortestPath(0, 6);
    correctPath = Path{0, 1, 3, 4, 6};
    CHECK(result.has_value());
    CHECK_EQ(result.value().path().size(), 5);
    CHECK(checkPath(result.value().path(), correctPath));
    CHECK_EQ(result.value().distance(), 19.);
  }

  SUBCASE("Case 6") {
    Street s1(0, 5, 7., std::make_pair(0, 1));
    Street s2(1, 5, 9., std::make_pair(0, 2));
    Street s3(2, 5, 14., std::make_pair(0, 5));
    Street s4(3, 5, 15., std::make_pair(1, 3));
    Street s5(4, 5, 10., std::make_pair(1, 2));
    Street s6(5, 5, 11., std::make_pair(2, 3));
    Street s7(6, 5, 2., std::make_pair(2, 5));
    Street s8(7, 5, 6., std::make_pair(3, 4));
    Street s9(8, 5, 9., std::make_pair(5, 4));
    Street s10(9, 5, 7., std::make_pair(1, 0));
    Street s11(10, 5, 9., std::make_pair(2, 0));
    Street s12(11, 5, 14., std::make_pair(5, 0));
    Street s13(12, 5, 15., std::make_pair(3, 1));
    Street s14(13, 5, 10., std::make_pair(2, 1));
    Street s15(14, 5, 11., std::make_pair(3, 2));
    Street s16(15, 5, 2., std::make_pair(5, 2));
    Street s17(16, 5, 6., std::make_pair(4, 3));
    Street s18(17, 5, 9., std::make_pair(4, 5));
    Graph graph{};
    graph.addStreets(
        s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18);
    graph.buildAdj();
    auto result = graph.shortestPath(0, 4);
    Path correctPath{0, 2, 5, 4};
    CHECK(result.has_value());
    CHECK_EQ(result.value().path().size(), 4);
    CHECK(checkPath(result.value().path(), correctPath));
    CHECK_EQ(result.value().distance(), 20.);
  }

  SUBCASE("Case 7") {
    Street s1(0, 5, 1., std::make_pair(1, 2));
    Street s2(1, 5, 6., std::make_pair(0, 2));
    Street s3(2, 5, 6., std::make_pair(2, 0));
    Graph graph{};
    graph.addStreets(s1, s2, s3);
    graph.buildAdj();
    auto result = graph.shortestPath(0, 1);
    CHECK_FALSE(result.has_value());
  }

  SUBCASE("Case 8") {
    Street s1(0, 5, 1., std::make_pair(1, 2));
    Street s2(1, 5, 6., std::make_pair(0, 2));
    Street s3(2, 5, 6., std::make_pair(2, 0));
    Graph graph{};
    graph.addStreets(s1, s2, s3);
    graph.buildAdj();
    auto result = graph.shortestPath(3, 1);
    CHECK_FALSE(result.has_value());
  }

  SUBCASE("Case 9") {
    Street s1(0, 5, 1., std::make_pair(1, 2));
    Street s2(1, 5, 6., std::make_pair(0, 2));
    Street s3(2, 5, 6., std::make_pair(2, 0));
    Graph graph{};
    graph.addStreets(s1, s2, s3);
    graph.buildAdj();
    auto result = graph.shortestPath(1, 3);
    CHECK_FALSE(result.has_value());
  }

  SUBCASE("street") {
    /// GIVEN: a graph
    /// WHEN: we add a street
    /// THEN: the street is added
    Graph graph{};
    Street street{1, 1, 1., std::make_pair(0, 1)};
    graph.addStreet(street);
    auto result = graph.street(0, 1);
    CHECK(result);
    const auto& street2 = *result;
    CHECK_EQ(street2->id(), 1);
    CHECK_EQ(street2->length(), 1.);
    CHECK_EQ(street2->capacity(), 1);
    CHECK_FALSE(graph.street(1, 0));
  }

  SUBCASE("equal length") {
    Graph graph{};
    graph.importMatrix("./data/matrix.dat", false);
    // check correct import
    CHECK_EQ(graph.adjMatrix().max_size(), 14400);
    CHECK_EQ(graph.adjMatrix().getRowDim(), 120);
    CHECK_EQ(graph.adjMatrix().getColDim(), 120);
    CHECK_EQ(graph.adjMatrix().size(), 436);
    // check that the path exists
    CHECK(graph.adjMatrix().operator()(46, 58));
    CHECK(graph.adjMatrix().operator()(58, 70));
    CHECK(graph.adjMatrix().operator()(70, 82));
    CHECK(graph.adjMatrix().operator()(82, 94));
    CHECK(graph.adjMatrix().operator()(94, 106));
    CHECK(graph.adjMatrix().operator()(106, 118));

    auto result = graph.shortestPath(46, 118);
    CHECK(result.has_value());
  }
}
