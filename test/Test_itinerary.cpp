#include <cstdint>

#include "Itinerary.hpp"

#include "doctest.h"

using Itinerary = dsm::Itinerary<uint8_t>;

TEST_CASE("Itinerary") {
  SUBCASE("Constructors") {
    GIVEN("An itinerary and its destination ids") {
      uint8_t itineraryId{0};
      uint8_t destinationId{2};
      WHEN("the Itinerary is constructed") {
        Itinerary itinerary{itineraryId, destinationId};
        THEN("The source and destination are set correctly") {
          CHECK_EQ(itinerary.id(), itineraryId);
          CHECK_EQ(itinerary.destination(), destinationId);
        }
      }
    }
    GIVEN("An itinerary id, its destination id and a transition matrix") {
      uint8_t itineraryId{0};
      uint8_t destinationId{2};
      dsm::SparseMatrix<uint8_t, bool> path{1, 1};
      WHEN("the Itinerary is constructed") {
        Itinerary itinerary{itineraryId, destinationId, path};
        THEN("The source, destination, and path are set correctly") {
          CHECK_EQ(itinerary.id(), itineraryId);
          CHECK_EQ(itinerary.destination(), destinationId);
          CHECK_EQ(itinerary.path().getRowDim(), 1);
          CHECK_EQ(itinerary.path().getColDim(), 1);
        }
      }
    }
  }
  SUBCASE("Set destination") {
    GIVEN("An itinerary id, its destination id and a transition matrix") {
      uint8_t itineraryId{0};
      uint8_t destinationId{2};
      dsm::SparseMatrix<uint8_t, bool> path{1, 1};
      Itinerary itinerary{itineraryId, destinationId, path};
      WHEN("the destination is set") {
        uint8_t newDestinationId{3};
        itinerary.setDestination(newDestinationId);
        THEN("The destination is set correctly and the path is cleared") {
          CHECK_EQ(itinerary.destination(), newDestinationId);
          CHECK_EQ(itinerary.path().getRowDim(), 0);
          CHECK_EQ(itinerary.path().getColDim(), 0);
          CHECK_EQ(itinerary.path().size(), 0);
        }
      }
    }
  }
}
