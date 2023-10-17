#include "../src/Street.hpp"
#include <cstdint>

#include "doctest.h"

using Street = dmf::Street<uint16_t, uint16_t>;

TEST_CASE("Street") {
  SUBCASE("Constructor") {
    /*This tests the constructor that takes an Id, capacity, and length.
    GIVEN: An Id, capacity, and length
    WHEN: A Street is constructed
    THEN: The Id, capacity, and length are set correctly
    */
    Street street{1, 2, 3.5};
    CHECK(street.id() == 1);
    CHECK(street.capacity() == 2);
    CHECK(street.length() == 3.5);
  }
}