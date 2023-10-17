#include <cstdint>

#include "Itinerary.hpp"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

using Itinerary = dmf::Itinerary<uint16_t>;

TEST_CASE("Itinerary") {
  SUBCASE("Constructor") {
    /*This tests the constructor that takes two Ids.
    GIVEN: Two Ids
    WHEN: An Itinerary is constructed
    THEN: The source and destination are set correctly
    */
    Itinerary itinerary{1, 2};
    CHECK(itinerary.source() == 1);
    CHECK(itinerary.destination() == 2);
  }
  SUBCASE("Copy constructor") {
    /*This tests the copy constructor.
    GIVEN: An Itinerary
    WHEN: An Itinerary is constructed from the first Itinerary
    THEN: The source and destination are set correctly
    */
    Itinerary itinerary{1, 2};
    Itinerary copy{itinerary};
    CHECK(copy.source() == 1);
    CHECK(copy.destination() == 2);
  }
}