#ifndef dsm_hpp
#define dsm_hpp

#include <cstdint>

static constexpr uint8_t DSM_VERSION_MAJOR = 1;
static constexpr uint8_t DSM_VERSION_MINOR = 3;
static constexpr uint8_t DSM_VERSION_PATCH = 3;
static constexpr uint8_t DSM_VERSION_BUILD = 6;

#include <string>

namespace dsm {
    std::string version();
}

std::string dsm::version() {
    return std::to_string(DSM_VERSION_MAJOR) + "." + std::to_string(DSM_VERSION_MINOR) + "." + std::to_string(DSM_VERSION_PATCH) + "." + std::to_string(DSM_VERSION_BUILD);
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
