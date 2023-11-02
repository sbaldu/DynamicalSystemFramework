#include <cstdint>

#include "Node.hpp"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

using Node = dsm::Node<uint16_t>;

TEST_CASE("Node") {
  SUBCASE("Constructor") {
    /*This tests the constructor that takes an Id.
    GIVEN: An Id
    WHEN: A Node is constructed
    THEN: The Id is set correctly
    */
    Node node{1};
    CHECK(node.id() == 1);
  }
  SUBCASE("Constructor") {
    /*This tests the constructor that takes an Id and coordinates.
    GIVEN: An Id and coordinates
    WHEN: A Node is constructed
    THEN: The Id and coordinates are set correctly
    */
    Node node{1, std::make_pair(2.5, 3.5)};
    CHECK(node.id() == 1);
    CHECK(node.coords().first == 2.5);
    CHECK(node.coords().second == 3.5);
  }
  SUBCASE("Constructor") {
    /*This tests the constructor that takes an Id, coordinates, and a queue.
        GIVEN: An Id, coordinates, and a queue
        WHEN: A Node is constructed
        THEN: The Id, coordinates, and queue are set correctly
        */
    std::queue<uint16_t> queue;
    queue.push(2);
    queue.push(3);
    Node node{1, std::make_pair(2.5, 3.5), queue};
    CHECK(node.id() == 1);
    CHECK(node.coords().first == 2.5);
    CHECK(node.coords().second == 3.5);
    CHECK(node.queue().front() == 2);
    CHECK(node.queue().back() == 3);
  }
}
