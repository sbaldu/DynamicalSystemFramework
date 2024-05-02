
#ifndef dijkstra_weights_hpp
#define dijkstra_weights_hpp

namespace dsm {

  template <typename Id, typename Size>
	requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  class Graph;

  template <typename Id, typename Size>
  double streetLength(const Graph<Id, Size>* graph, Id node1, Id node2) {
    return (*(graph->street(node1, node2)))->length();
  }

};  // namespace dsm

#endif
