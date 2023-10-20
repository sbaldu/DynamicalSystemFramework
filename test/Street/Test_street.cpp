
#include <cstdint>
#include <optional>

#include "Agent.hpp"
#include "Node.hpp"
#include "Street.hpp"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

using Agent = dmf::Agent<uint16_t, float>;
using Node = dmf::Node<uint16_t>;
using Street = dmf::Street<uint16_t, uint16_t>;

TEST_CASE("Street") {
  SUBCASE("Constructor_1") {
    /*This tests the constructor that takes an Id, capacity, and length.
    GIVEN: An Id, capacity, and length
    WHEN: A Street is constructed
    THEN: The Id, capacity, and length are set correctly
    */

    Street street{1, 2, 3.5};
    CHECK_EQ(street.id(), 1);
    CHECK_EQ(street.capacity(), 2);
    CHECK_EQ(street.length(), 3.5);
	CHECK_EQ(street.nodePair(), std::pair<uint16_t, uint16_t>());
  }

  SUBCASE("Constructor_2") {
    /*This tests the constructor that takes an Id, capacity, length, and nodePair.
    GIVEN: An Id, capacity, length and nodePair
    WHEN: A Street is constructed
    THEN: The Id, capacity, length and nodePair are set correctly
    */

    Street street{1, 2, 3.5, std::make_pair(4, 5)};
    CHECK_EQ(street.id(), 1);
    CHECK_EQ(street.capacity(), 2);
    CHECK_EQ(street.length(), 3.5);
	CHECK_EQ(street.nodePair().first, 4);
	CHECK_EQ(street.nodePair().second, 5);
  }

  SUBCASE("SetNodePair_1") {
    /*This tests the setNodePair method*/

    Street street{1, 2, 3.5};
    street.setNodePair(4, 5);
    CHECK_EQ(street.nodePair().first, 4);
    CHECK_EQ(street.nodePair().second, 5);
  }

  SUBCASE("SetNodePair_2") {
    /*This tests the setNodePair method*/

    Street street{1, 2, 3.5};
	Node node1{4};
	Node node2{5};
    street.setNodePair(node1, node2);
    CHECK_EQ(street.nodePair().first, 4);
    CHECK_EQ(street.nodePair().second, 5);
  }

  SUBCASE("SetNodePair_3") {
    /*This tests the setNodePair method*/

    Street street{1, 2, 3.5};
    street.setNodePair(std::make_pair(4, 5));
    CHECK_EQ(street.nodePair().first, 4);
    CHECK_EQ(street.nodePair().second, 5);
  }

  SUBCASE("Enqueue") {
    /*This tests the insertion of an agent in a street's queue*/

    // define some agents
    Agent a1{1, 1};  // they are all in street 1
    Agent a2{2, 1};
    Agent a3{3, 1};
    Agent a4{4, 1};

    Street street{1, 4, 3.5};
    // fill the queue
    street.enqueue(a1);
    street.enqueue(a2);
    street.enqueue(a3);
    street.enqueue(a4);
    CHECK_EQ(street.queue().front(), 1);
    CHECK_EQ(street.queue().back(), 4);
    CHECK_EQ(street.queue().size(), street.capacity());
    CHECK_EQ(street.size(), street.capacity());
  }

  SUBCASE("Dequeue") {
    /*This tests the exit of an agent from a street's queue*/

    // define some agents
    Agent a1{1, 1};  // they are all in street 1
    Agent a2{2, 1};
    Agent a3{3, 1};
    Agent a4{4, 1};

    Street street{1, 4, 3.5};
    // fill the queue
    street.enqueue(a1);
    street.enqueue(a2);
    street.enqueue(a3);
    street.enqueue(a4);
    CHECK_EQ(street.queue().front(), 1);  // check that agent 1 is at the front of the queue

    // dequeue
    street.dequeue();
    CHECK_EQ(street.queue().front(), 2);  // check that agent 2 is now at front
    // check that the length of the queue has decreased
    CHECK_EQ(street.queue().size(), 3);
    CHECK_EQ(street.size(), 3);
    // check that the next agent dequeued is agent 2
    CHECK_EQ(street.dequeue().value(), 2);
    CHECK_EQ(street.size(), 2);
    street.dequeue();
    street.dequeue();  // the queue is now empty
    // check that the result of dequeue is std::nullopt
    CHECK_FALSE(street.dequeue().has_value());
  }
}
