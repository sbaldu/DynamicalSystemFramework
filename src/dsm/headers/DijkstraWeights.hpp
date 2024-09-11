
#ifndef dijkstra_weights_hpp
#define dijkstra_weights_hpp

namespace dsm {

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  class Graph;

  template <typename Id, typename Size>
  double streetLength(const Graph<Id, Size>* graph, Id node1, Id node2) {
	const auto street{graph->street(node1, node2)};
    return (*street)->length();
  }

  template <typename Id, typename Size>
  double streetTime(const Graph<Id, Size>* graph, Id node1, Id node2) {
	const auto street{graph->street(node1, node2)};
	const auto length{(*street)->length()};
	const auto speed{(*street)->maxSpeed() *
                 (1. - (*street)->nAgents() / (*street)->capacity())};

	return length / speed;
  }


};  // namespace dsm

#endif
