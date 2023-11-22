
#include <cassert>
#include <cstdint>

#include "Graph.hpp"
#include "Node.hpp"
#include "Street.hpp"
#include "SparseMatrix.hpp"

#include "doctest.h"

using Graph = dsm::Graph<uint16_t, uint16_t>;
using SparseMatrix = dsm::SparseMatrix<uint16_t, bool>;
using Street = dsm::Street<uint16_t, uint16_t>;
using Path = std::vector<uint16_t>;

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

TEST_CASE("Dijkstra") {
  SUBCASE("Case 1") {
    Street s1{0, 5, 3., std::make_pair(0, 1)};
    Street s2{1, 5, 2., std::make_pair(1, 2)};
    Street s3{2, 5, 4., std::make_pair(2, 3)};
    Street s4{3, 5, 5., std::make_pair(3, 0)};
    Street s5{4, 5, 6., std::make_pair(0, 2)};
    Graph graph{};
    graph.addStreets(s1,s2,s3,s4,s5);
    graph.buildAdj();
    auto result = graph.dijkstra(0, 1);
    CHECK(result.value().getPath().size() == 2);
    CHECK(result.value().getDistance() == 3.);
    result = graph.dijkstra(0, 2);
    CHECK(result.value().getPath().size() == 3);
    CHECK(result.value().getDistance() == 5.);
  }

  SUBCASE("Case 2") {
	Street s1(0, 5, 1., std::make_pair(0, 1));
	Street s2(1, 5, 1., std::make_pair(1, 2));
	Street s3(2, 5, 6., std::make_pair(0, 2));
	Graph graph{};
	graph.addStreets(s1,s2,s3);
	graph.buildAdj();
	auto result = graph.dijkstra(0, 2);
  }
  
  SUBCASE("Case 3") {
	Street s1(0, 5, 5., std::make_pair(0, 1));
	Street s2(1, 5, 4., std::make_pair(1, 2));
	Street s3(2, 5, 6., std::make_pair(0, 2));
	Graph graph{};
	graph.addStreets(s1,s2,s3);
	graph.buildAdj();
	auto result = graph.dijkstra(0, 2);
  }

  SUBCASE("Case 4") {
	Street s1(0, 5, 3., std::make_pair(0, 1));
	Street s2(1, 5, 1., std::make_pair(0, 2));
	Street s3(2, 5, 7., std::make_pair(1, 2));
	Street s4(3, 5, 2., std::make_pair(2, 3));
	Street s5(4, 5, 1., std::make_pair(1, 4));
	Street s6(5, 5, 5., std::make_pair(1, 3));
	Street s7(6, 5, 7., std::make_pair(3, 4));
	Street s8(0, 5, 3., std::make_pair(1, 0));
	Street s9(1, 5, 1., std::make_pair(2, 0));
	Street s10(2, 5, 7., std::make_pair(2, 1));
	Street s11(3, 5, 2., std::make_pair(3, 2));
	Street s12(4, 5, 1., std::make_pair(4, 1));
	Street s13(5, 5, 5., std::make_pair(3, 1));
	Street s14(6, 5, 7., std::make_pair(4, 3));
	Graph graph{};
	graph.addStreets(s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11,s12,s13,s14);
	graph.buildAdj();
	auto result = graph.dijkstra(2, 4);
	result = graph.dijkstra(2, 1);
	result = graph.dijkstra(2, 3);
  }

  SUBCASE("Case 5") {
	Street s1(0, 5, 2., std::make_pair(0, 1));
	Street s2(1, 5, 6., std::make_pair(0, 2));
	Street s3(2, 5, 5., std::make_pair(1, 3));
	Street s4(3, 5, 8., std::make_pair(2, 3));
	Street s5(4, 5, 15., std::make_pair(3, 5));
	Street s6(5, 5, 10., std::make_pair(3, 4));
	Street s7(6, 5, 6., std::make_pair(4, 5));
	Street s8(0, 5, 2., std::make_pair(4, 6));
	Street s9(1, 5, 6., std::make_pair(5, 6));
	Street s10(0, 5, 2., std::make_pair(1, 0));
	Street s11(1, 5, 6., std::make_pair(2, 0));
	Street s12(2, 5, 5., std::make_pair(3, 1));
	Street s13(3, 5, 8., std::make_pair(3, 2));
	Street s14(4, 5, 15., std::make_pair(5, 3));
	Street s15(5, 5, 10., std::make_pair(4, 3));
	Street s16(6, 5, 6., std::make_pair(5, 4));
	Street s17(0, 5, 2., std::make_pair(6, 4));
	Street s18(1, 5, 6., std::make_pair(6, 5));
	Graph graph{};
	graph.addStreets(s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11,s12,s13,s14,s15,s16,s17,s18);
	graph.buildAdj();
	auto result = graph.dijkstra(0, 2);
	result = graph.dijkstra(0, 3);
	result = graph.dijkstra(0, 4);
	result = graph.dijkstra(0, 5);
	result = graph.dijkstra(0, 6);
  }

  SUBCASE("Case 6") {
	Street s1(0, 5, 7., std::make_pair(0, 1));
	Street s2(1, 5, 9., std::make_pair(0, 2));
	Street s3(2, 5, 14., std::make_pair(0, 5));
	Street s4(3, 5, 15., std::make_pair(1, 3));
	Street s5(4, 5, 10., std::make_pair(1, 2));
	Street s6(5, 5, 11., std::make_pair(2, 3));
	Street s7(6, 5, 2., std::make_pair(2, 5));
	Street s8(0, 5, 6., std::make_pair(3, 4));
	Street s9(1, 5, 9., std::make_pair(5, 4));
	Street s10(0, 5, 7., std::make_pair(1, 0));
	Street s11(1, 5, 9., std::make_pair(2, 0));
	Street s12(2, 5, 14., std::make_pair(5, 0));
	Street s13(3, 5, 15., std::make_pair(3, 1));
	Street s14(4, 5, 10., std::make_pair(2, 1));
	Street s15(5, 5, 11., std::make_pair(3, 2));
	Street s16(6, 5, 2., std::make_pair(5, 2));
	Street s17(0, 5, 6., std::make_pair(4, 3));
	Street s18(1, 5, 9., std::make_pair(4, 5));
	Graph graph{};
	graph.addStreets(s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11,s12,s13,s14,s15,s16,s17,s18);
	graph.buildAdj();
	auto result = graph.dijkstra(0, 4);
  }

  SUBCASE("Case 7") {
	Street s1(0, 5, 1., std::make_pair(1, 2));
	Street s2(1, 5, 6., std::make_pair(0, 2));
	Graph graph{};
	graph.addStreets(s1,s2,s3);
	graph.buildAdj();
	auto result = graph.dijkstra(0, 1);
	CHECK_FALSE(result.has_value());
  }

  SUBCASE("Case 8") {
	Street s1(0, 5, 1., std::make_pair(1, 2));
	Street s2(1, 5, 6., std::make_pair(0, 2));
	Street s3(2, 5, 6., std::make_pair(2, 0));
	Graph graph{};
	graph.addStreets(s1,s2,s3);
	graph.buildAdj();
	auto result = graph.dijkstra(3, 1);
	CHECK_FALSE(result.has_value());
  }

  SUBCASE("Case 9") {
	Street s1(0, 5, 1., std::make_pair(1, 2));
	Street s2(1, 5, 6., std::make_pair(0, 2));
	Street s3(2, 5, 6., std::make_pair(2, 0));
	Graph graph{};
	graph.addStreets(s1,s2,s3);
	graph.buildAdj();
	auto result = graph.dijkstra(1, 3);
	CHECK_FALSE(result.has_value());
  }
}
