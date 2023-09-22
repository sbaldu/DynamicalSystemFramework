
#ifndef Node_hpp
#define Node_hpp

#include <queue>

namespace dmf {

  template <typename Id, template <typename Id_> typename Container, typename Priority>
  class Node {
  private:
    std::priority_queue<Id, Container<Id>, Priority> m_queue;

  public:
    Node() = default;
    Node(std::priority_queue<Id> queue) : m_queue{std::move(queue)} {}
  };
};  // namespace dmf

#endif
