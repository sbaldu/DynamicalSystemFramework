#include <cstdint>

#include "Agent.hpp"
#include "Itinerary.hpp"

#include "doctest.h"

using Agent = dsm::Agent<uint16_t, uint16_t, uint16_t>;
using Itinerary = dsm::Itinerary<uint16_t>;

TEST_CASE("Agent") {
  SUBCASE("Constructor_1") {
    Agent agent{1, 1, 0};
    CHECK_EQ(agent.index(), 1);
    CHECK_EQ(agent.streetId(), 1);
    CHECK_EQ(agent.itineraryId(), 0);
    CHECK_EQ(agent.speed(), 0);
    CHECK_EQ(agent.delay(), 0);
    CHECK_EQ(agent.time(), 0);
  }
}