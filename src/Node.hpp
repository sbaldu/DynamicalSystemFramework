
#ifndef Node_hpp
#define Node_hpp

#include <functional>
#include <queue>

namespace dmf {

  template <typename Id>
  class Node {
  private:
    Id m_id;
    std::queue<Id> m_queue;

  public:
    Node() = default;
    Node(std::queue<Id> queue) : m_queue{std::move(queue)} {}

    Id id() const { return m_id; }
  };

  // to be implemented
  /* template <typename Id> */
  /* class Intersection : public Node<Id> { */
  /* private: */
  /*   std::function<void()> m_priority; */
  /* }; */

  /* template <typename Id> */
  /* class Roundabout : public Node<Id> { */
  /* private: */
  /*   std::function<void()> m_priority; */
  /* }; */

  /* template <typename Id> */
  /* class TrafficLight : public Node<Id> { */
  /* private: */
  /*   std::function<void()> m_priority; */
  /* }; */
};  // namespace dmf

#endif
