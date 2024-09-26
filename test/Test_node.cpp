#include <cstdint>

#include "Node.hpp"

#include "doctest.h"

using Intersection = dsm::Intersection<uint16_t, uint16_t>;
using TrafficLight = dsm::TrafficLight<uint16_t, uint16_t, uint16_t>;

TEST_CASE("Intersection") {
  SUBCASE("Constructor") {
    /*This tests the constructor that takes an Id.
    GIVEN: An Id
    WHEN: A Intersection is constructed
    THEN: The Id is set correctly
    */
    Intersection node{1};
    CHECK(node.id() == 1);
  }
  SUBCASE("Constructor") {
    /*This tests the constructor that takes an Id and coordinates.
    GIVEN: An Id and coordinates
    WHEN: A Intersection is constructed
    THEN: The Id and coordinates are set correctly
    */
    Intersection node{1, std::make_pair(2.5, 3.5)};
    CHECK(node.id() == 1);
    CHECK(node.coords().value().first == 2.5);
    CHECK(node.coords().value().second == 3.5);
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
    /// WHEN: The phase is set to update after a green-red cycle
    /// THEN: It's checked that the current gree-red cycle is not affected
    /// and ultimately it's checked that on the next green-red cycle the phase is updated correctly
    TrafficLight trafficLight{0};
    trafficLight.setDelay(std::make_pair(5, 7));
    trafficLight.setPhaseAfterCycle(6);

    for (size_t i = 0; i < 12; ++i) {
      trafficLight.increaseCounter();
    }
    CHECK_FALSE(trafficLight.isGreen());
    trafficLight.increaseCounter();
    CHECK_FALSE(trafficLight.isGreen());

    trafficLight.setPhase(0);
    CHECK(trafficLight.isGreen());

    for (size_t i = 0; i < 12; ++i) {
      trafficLight.increaseCounter();
    }
    CHECK(trafficLight.isGreen());
  }
  SUBCASE("Asymmetric traffic light") {
    /// This tests the asymmetric traffic light.
    /// GIVEN: A TrafficLight
    /// WHEN: The asymmetric traffic light is set
    /// THEN: The asymmetric traffic light is set correctly
    TrafficLight trafficLight{0};
    trafficLight.setDelay(std::make_pair(5, 3));
    for (size_t i = 0; i < 8; ++i) {
      if (i < 5) {
        CHECK(trafficLight.isGreen());
      } else {
        CHECK_FALSE(trafficLight.isGreen());
      }
      trafficLight.increaseCounter();
    }
    CHECK(trafficLight.isGreen());
  }
  SUBCASE("Dynamic traffic light") {
    GIVEN("A traffic ligth object with set delay") {
      TrafficLight tl{0};
      tl.setDelay(std::make_pair(5, 3));
      WHEN("The delay is set with a green value smaller than the previous one") {
        tl.increaseCounter();
        tl.increaseCounter();
        tl.increaseCounter();
        tl.setDelay(std::make_pair(2, 3));
        THEN("It is green for two cycles") {
          CHECK(tl.isGreen());
          tl.increaseCounter();
          CHECK(tl.isGreen());
          tl.increaseCounter();
          CHECK_FALSE(tl.isGreen());
        }
      }
      WHEN("The delay is set with a red value smaller than the previous one") {
        tl.increaseCounter();
        tl.increaseCounter();
        tl.increaseCounter();
        tl.increaseCounter();
        tl.increaseCounter();
        tl.setDelay(std::make_pair(1, 3));
        THEN("It is red for one cycles") {
          CHECK_FALSE(tl.isGreen());
          tl.increaseCounter();
          CHECK(tl.isGreen());
        }
      }
    }
  }
}
