
#pragma once

#include "../utility/Typedef.hpp"

namespace dsm {

  class Graph;

  double streetLength(const Graph* graph, Id node1, Id node2);
  double streetTime(const Graph* graph, Id node1, Id node2);

};  // namespace dsm
