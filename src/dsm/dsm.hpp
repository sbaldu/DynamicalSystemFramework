#ifndef dsm_hpp
#define dsm_hpp

#include <cstdint>
#include <format>

static constexpr uint8_t DSM_VERSION_MAJOR = 2;
static constexpr uint8_t DSM_VERSION_MINOR = 0;
static constexpr uint8_t DSM_VERSION_PATCH = 0;

namespace dsm {
  constexpr const char* version();
}

constexpr const char* dsm::version() {
  return std::format("{}.{}.{}", DSM_VERSION_MAJOR, DSM_VERSION_MINOR, DSM_VERSION_PATCH)
      .c_str();
}

#include "headers/Agent.hpp"
#include "headers/Graph.hpp"
#include "headers/Itinerary.hpp"
#include "headers/Node.hpp"
#include "headers/SparseMatrix.hpp"
#include "headers/Street.hpp"
#include "headers/Dynamics.hpp"
#include "utility/TypeTraits/is_node.hpp"
#include "utility/TypeTraits/is_street.hpp"
#include "utility/TypeTraits/is_numeric.hpp"

#endif
