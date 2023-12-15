#include <cstdint>

#include "Node.hpp"

#include "doctest.h"

using Node = dsm::Node<uint16_t, uint16_t>;
using TrafficLight = dsm::TrafficLight<uint16_t, uint16_t, uint16_t>;

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
  SUBCASE("queue management") {
    /*This tests the queue management functions.
    GIVEN: A Node
    WHEN: The queue is set, an id is enqueued, and an id is dequeued
    THEN: The queue is set correctly, the id is enqueued correctly, and the id is dequeued correctly
    */
    Node node{1};
    std::queue<uint16_t> queue;
    CHECK_THROWS(node.dequeue());
    queue.push(2);
    queue.push(3);
    CHECK_THROWS(node.setQueue(queue));
    node.enqueue(2);
    CHECK_THROWS(node.enqueue(3));
    CHECK(node.isFull());
    node.setCapacity(2);
    CHECK_FALSE(node.isFull());
    node.enqueue(3);
    CHECK_EQ(node.dequeue(), 2);
    CHECK_EQ(node.dequeue(), 3);
  }
}

TEST_CASE("TrafficLight") {
  SUBCASE("Constructor") {
    /// This tests the constructor that takes an Id.
    /// GIVEN: An Id
    /// WHEN: A TrafficLight is constructed
    /// THEN: The Id is set correctly
    TrafficLight trafficLight{1};
    CHECK(trafficLight.id() == 1);
  }
  SUBCASE("Light cycle") {
    /// This tests the light cycle.
    /// GIVEN: A TrafficLight
    /// WHEN: The light cycle is set
    /// THEN: The light cycle is set correctly
    TrafficLight trafficLight{0};
    trafficLight.setDelay(1);
    CHECK(trafficLight.isGreen());
    trafficLight.increaseCounter();
    CHECK_FALSE(trafficLight.isGreen());
    trafficLight.increaseCounter();
    CHECK(trafficLight.isGreen());
  }
  SUBCASE("Ligh cycle 2") {
    /// This tests the light cycle.
    /// GIVEN: A TrafficLight
    /// WHEN: The light cycle is set
    /// THEN: The light cycle is set correctly
    TrafficLight trafficLight{0};
    trafficLight.setDelay(2);
    CHECK(trafficLight.isGreen());
    trafficLight.increaseCounter();
    CHECK(trafficLight.isGreen());
    trafficLight.increaseCounter();
    CHECK_FALSE(trafficLight.isGreen());
    trafficLight.increaseCounter();
    CHECK_FALSE(trafficLight.isGreen());
    trafficLight.increaseCounter();
    CHECK(trafficLight.isGreen());
  }
  SUBCASE("Phase") {
    /// This tests the phase.
    /// GIVEN: A TrafficLight
    /// WHEN: The phase is set
    /// THEN: The phase is set correctly
    TrafficLight trafficLight{0};
    trafficLight.setDelay(5);
    trafficLight.setPhase(5);
    CHECK(trafficLight.isGreen());
    trafficLight.setPhase(7);
    CHECK_FALSE(trafficLight.isGreen());
  }
}
