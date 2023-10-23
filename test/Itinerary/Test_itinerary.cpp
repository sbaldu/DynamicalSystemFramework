#include <cstdint>

#include "Itinerary.hpp"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

using Itinerary = dmf::Itinerary<uint16_t>;

TEST_CASE("Itinerary") {
  SUBCASE("Constructor_1") {
    /*This tests the constructor that takes two Ids.
    GIVEN: Two Ids
    WHEN: An Itinerary is constructed
    THEN: The source and destination are set correctly
    */
    Itinerary itinerary{1, 2};
    CHECK(itinerary.source() == 1);
    CHECK(itinerary.destination() == 2);
  }
  SUBCASE("Constructor_2") {
    /*This tests the constructor that takes a pair of Ids.
    GIVEN: A pair of Ids
    WHEN: An Itinerary is constructed
    THEN: The source and destination are set correctly
    */
    Itinerary itinerary{std::pair{1, 2}};
    CHECK(itinerary.source() == 1);
    CHECK(itinerary.destination() == 2);
  }
  SUBCASE("Constructor_3") {
    /*This tests the constructor that takes two Ids and a SparseMatrix.
    GIVEN: Two Ids and a SparseMatrix
    WHEN: An Itinerary is constructed
    THEN: The source, destination, and path are set correctly
    */
    Itinerary itinerary{1, 2, dmf::SparseMatrix<uint16_t, bool>{1, 1}};
    CHECK(itinerary.source() == 1);
    CHECK(itinerary.destination() == 2);
    CHECK(itinerary.path().getRowDim() == 1);
    CHECK(itinerary.path().getColDim() == 1);
  }
  SUBCASE("Constructor_4") {
    /*This tests the constructor that takes a pair of Ids and a SparseMatrix.
    GIVEN: A pair of Ids and a SparseMatrix
    WHEN: An Itinerary is constructed
    THEN: The source, destination, and path are set correctly
    */
    Itinerary itinerary{std::pair{1, 2}, dmf::SparseMatrix<uint16_t, bool>{1, 1}};
    CHECK(itinerary.source() == 1);
    CHECK(itinerary.destination() == 2);
    CHECK(itinerary.path().getRowDim() == 1);
    CHECK(itinerary.path().getColDim() == 1);
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