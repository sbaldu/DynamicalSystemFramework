#include <cstdint>

#include "Dynamics.hpp"
#include "Graph.hpp"
#include "Node.hpp"
#include "Street.hpp"
#include "SparseMatrix.hpp"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

using Dynamics = dsm::FirstOrderDynamics<uint16_t, uint16_t, uint16_t>;
using Graph = dsm::Graph<uint16_t, uint16_t>;
using SparseMatrix = dsm::SparseMatrix<uint16_t, bool>;
using Street = dsm::Street<uint16_t, uint16_t>;

TEST_CASE("Dynamics") {
    SUBCASE("Constructor") {
        // Dynamics dynamics;
        // CHECK(dynamics.getGraph().getNodes().empty());
        // CHECK(dynamics.getGraph().getStreets().empty());
        // CHECK(dynamics.getGraph().getAdjacency() == nullptr);
    }
}