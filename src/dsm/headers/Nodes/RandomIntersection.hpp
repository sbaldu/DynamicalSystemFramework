
#ifndef RandomIntersection_hpp
#define RandomIntersection_hpp

#include <concepts>
#include "../../utility/queue.hpp"

#include "Node.hpp"

namespace dsm {

  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  class RandomIntersection : public Node<Id, Size> {
  private:
    queue<Size> m_agentQueue;

  public:
    RandomIntersection() = delete;
    RandomIntersection(Id id);

    void enqueue(Size agentId);
    void dequeue();
    const dsm::queue<Size>& queue() const;

    bool isFull() const override;
    bool isFull(Id streetId) const override;
  };

  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  RandomIntersection<Id, Size>::RandomIntersection(Id id) : Node<Id, Size>{id} {}

  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  void RandomIntersection<Id, Size>::enqueue(Size agentId) {
    m_agentQueue.push(agentId);
  }

  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  void RandomIntersection<Id, Size>::dequeue() {
    m_agentQueue.pop();
  }

  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  const dsm::queue<Size>& RandomIntersection<Id, Size>::queue() const {
    return m_agentQueue;
  }

  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  bool RandomIntersection<Id, Size>::isFull() const {}

  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  bool RandomIntersection<Id, Size>::isFull(Id streetId) const {}

};  // namespace dsm

#endif
