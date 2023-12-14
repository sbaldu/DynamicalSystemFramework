#include <cstdint>

#include "Agent.hpp"
#include "Itinerary.hpp"

#include "doctest.h"

using Agent = dsm::Agent<uint16_t, uint16_t, uint16_t>;
using Itinerary = dsm::Itinerary<uint16_t>;

TEST_CASE("Agent") {
  SUBCASE("Constructor_1") {
    Agent agent{1, 0};
    CHECK_EQ(agent.id(), 1);
    CHECK_EQ(agent.itineraryId(), 0);
    CHECK_FALSE(agent.streetId().has_value());
    CHECK_EQ(agent.speed(), 0);
    CHECK_EQ(agent.delay(), 0);
    CHECK_EQ(agent.time(), 0);
  }
  SUBCASE("Constructor_2") {
    Agent agent{1, 0, 0};
    CHECK_EQ(agent.id(), 1);
    CHECK_EQ(agent.itineraryId(), 0);
    CHECK(agent.streetId().has_value());
    CHECK_EQ(agent.streetId().value(), 0);
    CHECK_EQ(agent.speed(), 0);
    CHECK_EQ(agent.delay(), 0);
    CHECK_EQ(agent.time(), 0);
  }
}