#include <cstdint>

#include "Agent.hpp"
#include "Itinerary.hpp"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

using Agent = dmf::Agent<uint16_t, double>;
using Itinerary = dmf::Itinerary<uint16_t>;

TEST_CASE("Agent") {
  /*This tests the constructor of the Agent class.
    GIVEN a valid index, position, and itinerary
    WHEN the constructor is called
    THEN the index, position, and previousPosition are set to the given values
    */
  SUBCASE("Constructor") {
    Agent agent{0, 0, 0};
    CHECK(agent.index() == 0);
    CHECK(agent.position() == 0);
    CHECK(agent.previousPosition() == 0);
  }
}