#include "../src/Agent.hpp"
#include "../src/Itinerary.hpp"

#include "doctest.h"

using Agent = dmf::Agent<uint16_t>;
using Itinerary = dmf::Itinerary<uint16_t>;

TEST_CASE("Agent") {
    Agent::addItinerary(Itinerary{0, 1});
    SUBCASE("Constructor") {
        Agent agent{0, 0};
        CHECK(agent.index() == 0);
        CHECK(agent.position() == 0);
        CHECK(agent.previousPosition() == 0);
        CHECK(agent.itinerary().source() == 0);
        CHECK(agent.itinerary().destination() == 1);
    }
}