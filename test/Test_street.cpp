#include <cstdint>
#include <optional>
#include <numbers>

#include "Agent.hpp"
#include "Node.hpp"
#include "Street.hpp"

#include "doctest.h"

using Agent = dsm::Agent<uint16_t, uint16_t, double>;
using Node = dsm::Node<uint16_t, uint16_t>;
using Street = dsm::Street<uint16_t, uint16_t>;

TEST_CASE("Street") {
  SUBCASE("Constructor_1") {
    /*This tests the constructor that takes an Id, capacity, and length.
    GIVEN: An Id, capacity, and length
    WHEN: A Street is constructed
    THEN: The Id, capacity, and length are set correctly
    */

    Street street{1, std::make_pair(0, 1)};
    CHECK_EQ(street.id(), 1);
    CHECK_EQ(street.capacity(), 1);
    CHECK_EQ(street.transportCapacity(), 65535);
    CHECK_EQ(street.length(), 1.);
    CHECK_EQ(street.nodePair().first, 0);
    CHECK_EQ(street.nodePair().second, 1);
    CHECK_EQ(street.maxSpeed(), 13.8888888889);
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
    CHECK_EQ(street.transportCapacity(), 65535);
    CHECK_EQ(street.length(), 3.5);
    CHECK_EQ(street.nodePair().first, 4);
    CHECK_EQ(street.nodePair().second, 5);
    CHECK_EQ(street.density(), 0);
    CHECK_EQ(street.maxSpeed(), 13.8888888889);
  }
  SUBCASE("Constructor_3") {
    /*This tests the constructor that takes an Id, capacity, length, nodePair, and maxSpeed.
    GIVEN: An Id, capacity, length, nodePair, and maxSpeed
    WHEN: A Street is constructed
    THEN: The Id, capacity, length, nodePair, and maxSpeed are set correctly
    */

    Street street{1, 2, 3.5, 40., std::make_pair(4, 5)};
    CHECK_EQ(street.id(), 1);
    CHECK_EQ(street.capacity(), 2);
    CHECK_EQ(street.transportCapacity(), 65535);
    CHECK_EQ(street.length(), 3.5);
    CHECK_EQ(street.nodePair().first, 4);
    CHECK_EQ(street.nodePair().second, 5);
    CHECK_EQ(street.density(), 0);
    CHECK_EQ(street.maxSpeed(), 40.);
  }

  SUBCASE("SetNodePair_1") {
    /*This tests the setNodePair method*/

    Street street{1, std::make_pair(0, 1)};
    street.setNodePair(4, 5);
    CHECK_EQ(street.nodePair().first, 4);
    CHECK_EQ(street.nodePair().second, 5);
  }

  SUBCASE("SetNodePair_2") {
    /*This tests the setNodePair method*/

    Street street{1, std::make_pair(0, 1)};
    Node node1{4};
    Node node2{5};
    street.setNodePair(node1, node2);
    CHECK_EQ(street.nodePair().first, 4);
    CHECK_EQ(street.nodePair().second, 5);
  }

  SUBCASE("SetNodePair_3") {
    /*This tests the setNodePair method*/

    Street street{1, std::make_pair(0, 1)};
    street.setNodePair(std::make_pair(4, 5));
    CHECK_EQ(street.nodePair().first, 4);
    CHECK_EQ(street.nodePair().second, 5);
  }

  SUBCASE("Enqueue") {
    /*This tests the insertion of an agent in a street's queue*/

    // define some agents
    Agent a1{1, 1, 0};  // they are all in street 1
    Agent a2{2, 1, 0};
    Agent a3{3, 1, 0};
    Agent a4{4, 1, 0};

    Street street{1, 4, 3.5, std::make_pair(0, 1)};
    // fill the queue
    street.enqueue(a1.id());
    street.enqueue(a2.id());
    CHECK_EQ(street.density(), 0.5);
    street.enqueue(a3.id());
    street.enqueue(a4.id());
    CHECK_EQ(street.queue().front(), 1);
    CHECK_EQ(street.queue().back(), 4);
    CHECK_EQ(street.queue().size(), street.capacity());
    CHECK_EQ(street.queue().size(), street.capacity());
    CHECK_EQ(street.density(), 1);
  }

  SUBCASE("Dequeue") {
    /*This tests the exit of an agent from a street's queue*/

    // define some agents
    Agent a1{1, 1, 0};  // they are all in street 1
    Agent a2{2, 1, 0};
    Agent a3{3, 1, 0};
    Agent a4{4, 1, 0};

    Street street{1, 4, 3.5, std::make_pair(0, 1)};
    // fill the queue
    street.enqueue(a1.id());
    street.enqueue(a2.id());
    street.enqueue(a3.id());
    street.enqueue(a4.id());
    CHECK_EQ(street.queue().front(),
             1);  // check that agent 1 is at the front of the queue
    // dequeue
    street.dequeue();
    CHECK_EQ(street.queue().front(), 2);  // check that agent 2 is now at front
    // check that the length of the queue has decreased
    CHECK_EQ(street.queue().size(), 3);
    CHECK_EQ(street.queue().size(), 3);
    // check that the next agent dequeued is agent 2
    CHECK_EQ(street.dequeue().value(), 2);
    CHECK_EQ(street.queue().size(), 2);
    street.dequeue();
    street.dequeue();  // the queue is now empty
    // check that the result of dequeue is std::nullopt
    CHECK_FALSE(street.dequeue().has_value());
  }
  SUBCASE("Angle") {
    /// This tests the angle method
    /// GIVEN: A street
    /// WHEN: The angle method is called
    /// THEN: The angle is returned and is correct
    Street street{1, 4, 3.5, std::make_pair(0, 1)};
    CHECK_EQ(street.angle(), 0);
    street.setAngle(std::make_pair(0, 1), std::make_pair(1, 0));
    CHECK_EQ(street.angle(), 7 * std::numbers::pi / 4);
    // exceptions
    CHECK_THROWS(street.setAngle(-0.1));
    CHECK_THROWS(street.setAngle(7.));
  }
}
