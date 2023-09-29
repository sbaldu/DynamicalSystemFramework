
#ifndef Graph_hpp
#define Graph_hpp

#include <queue>

#include "Street.hpp"

#include "CXXGraph.hpp"

namespace dmf {

  // We need to overload iostream operators for the priority queue
  template <typename Id, template <typename Id_> typename Container, typename Priority>
  std::ostream& operator<<(std::ostream& os,
                           const std::priority_queue<Id, Container<Id>, Priority>& queue) {
    for (size_t i{}; i < queue.size(); ++i) {
      os << "Index: " << i << ", " << queue[i] << '\n';
    }

    return os;
  }

  // Alias for shared pointers
  template <typename T>
  using shared = std::shared_ptr<T>;

  // Alias for CXXGraph objects
  using CXXGraph::Edge;

  // Alias for graph Node type
  template <typename Id, template <typename Id_> typename Container, typename Priority>
  using Intersection_t = std::priority_queue<Id, Container<Id>, Priority>;

  template <typename Id, template <typename Id_> typename Container, typename Priority>
  class Graph {
  private:
    CXXGraph::Graph<Intersection_t<Id, Container, Priority>> m_graph;

  public:
    Graph() = default;

    template <typename Size>
    Graph(std::unordered_set<shared<Street<Id, Size>>> streetSet);
  };

  template <typename Id, template <typename Id_> typename Container, typename Priority>
  template <typename Size>
  Graph<Id, Container, Priority>::Graph(std::unordered_set<shared<Street<Id, Size>>> streetSet) {
    CXXGraph::T_EdgeSet<Intersection_t<Id, Container, Priority>> edgeSet;

    for (auto streetIt : streetSet) {
      edgeSet.insert(
          std::static_pointer_cast<const CXXGraph::Edge<Intersection_t<Id, Container, Priority>>>(
              *streetIt));
    }

    m_graph = CXXGraph::Graph(edgeSet);
  }
};  // namespace dmf

#endif
