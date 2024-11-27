
#pragma once

#include <cstdint>

namespace dsm {

  using Id = uint32_t;
  using Size = uint32_t;
  using Delay = uint8_t;
  using Time = uint64_t;

  enum Direction : uint8_t {
    RIGHT = 0,     // delta < 0
    STRAIGHT = 1,  // delta == 0
    LEFT = 2,      // delta > 0
    UTURN = 3,     // std::abs(delta) > std::numbers::pi
    RIGHTANDSTRAIGHT = 4,
    LEFTANDSTRAIGHT = 5,
    ANY = 6
  };

};  // namespace dsm
