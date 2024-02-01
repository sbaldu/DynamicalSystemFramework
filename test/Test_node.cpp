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
  SUBCASE("isGreen exception") {
    /*This tests the isGreen function.
    GIVEN: A Node
    WHEN: The isGreen function is called
    THEN: An exception is thrown
    */
    Node node{1};
    CHECK_THROWS(node.isGreen());
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
  SUBCASE("Asymmetric traffic light") {
    /// This tests the asymmetric traffic light.
    /// GIVEN: A TrafficLight
    /// WHEN: The asymmetric traffic light is set
    /// THEN: The asymmetric traffic light is set correctly
    TrafficLight trafficLight{0};
    trafficLight.setDelay(std::make_pair(5, 3));
    for (int i = 0; i < 8; ++i) {
      if (i < 5) {
        CHECK(trafficLight.isGreen());
      } else {
        CHECK_FALSE(trafficLight.isGreen());
      }
      trafficLight.increaseCounter();
    }
    CHECK(trafficLight.isGreen());
  }
}