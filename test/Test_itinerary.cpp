#include <cstdint>

#include "Itinerary.hpp"

#include "doctest.h"

using Itinerary = dsm::Itinerary;

TEST_CASE("Itinerary") {
  SUBCASE("Constructors") {
    GIVEN("Some parameters") {
      dsm::Id itineraryId{0};
      dsm::Id destinationId{2};
      std::vector<dsm::Id> trip{0, 1, 2};
      WHEN("The Itinerary is constructed") {
        Itinerary itinerary{itineraryId, destinationId};
        THEN("The source and destination are set correctly") {
          CHECK_EQ(itinerary.id(), itineraryId);
          CHECK_EQ(itinerary.destination(), destinationId);
        }
      }
      WHEN("The Itinerary is constructed with a trip") {
        Itinerary itinerary{itineraryId, trip};
        THEN("The source and destination are set correctly") {
          CHECK_EQ(itinerary.id(), itineraryId);
          CHECK_EQ(itinerary.destination(), destinationId);
          CHECK_EQ(itinerary.trip(), trip);
        }
      }
    }
  }
}
