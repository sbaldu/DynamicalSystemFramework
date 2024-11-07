#ifndef dsm_hpp
#define dsm_hpp

#include <cstdint>

static constexpr uint8_t DSM_VERSION_MAJOR = 1;
static constexpr uint8_t DSM_VERSION_MINOR = 4;
static constexpr uint8_t DSM_VERSION_PATCH = 2;

#include <string>
#include <format>

namespace dsm {
  constexpr std::string_view version();
}

constexpr std::string_view dsm::version() {
  return std::format("{}.{}.{}", DSM_VERSION_MAJOR, DSM_VERSION_MINOR, DSM_VERSION_PATCH);
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
