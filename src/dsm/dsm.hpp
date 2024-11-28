#pragma once

#include <cstdint>
#include <format>

static constexpr uint8_t DSM_VERSION_MAJOR = 2;
static constexpr uint8_t DSM_VERSION_MINOR = 1;
static constexpr uint8_t DSM_VERSION_PATCH = 10;

static auto const DSM_VERSION =
    std::format("{}.{}.{}", DSM_VERSION_MAJOR, DSM_VERSION_MINOR, DSM_VERSION_PATCH);

namespace dsm {
  /// @brief Returns the version of the DSM library
  /// @return The version of the DSM library
  auto const& version() { return DSM_VERSION; };
}  // namespace dsm

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
