
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
    /// @brief Constructs a random intersection
    /// @param id The id of the intersection
    RandomIntersection(Id id);

    /// @brief Adds an agent to the queue
    /// @param agentId The id of the agent to add
    void enqueue(Size agentId);
    /// @brief Removes the first agent from the queue
    void dequeue();

    /// @brief Returns the queue of agents
    /// @return dsm::queue<Size>& The queue of agents
    const dsm::queue<Size>& queue() const;
    /// @brief Returns whether the queue is full
    /// @details The queue is considered full when it has reached its capacity
    /// @return bool Whether the queue is full
    bool isFull() const override;
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
  bool RandomIntersection<Id, Size>::isFull() const {
    return m_agentQueue.size() >= this->m_capacity;
  }

};  // namespace dsm

#endif
