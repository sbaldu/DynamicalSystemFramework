
#ifndef Intersection_hpp
#define Intersection_hpp

#include <concepts>
#include <type_traits>
#include "../../utility/queue.hpp"

#include "Node.hpp"

namespace dsm {

  template <typename Id, typename Size, typename Priority>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  class Intersection : public Node<Id, Size> {
  private:
    priority_queue<Id, Priority> m_queue;

  public:
    Intersection() = delete;
    Intersection(Id id);

    const dsm::priority_queue<Id, Priority>& queue() const;

    bool isFull() const override;
    bool isFull(Id streetId) const override;
  };

  template <typename Id, typename Size, typename Priority>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  Intersection<Id, Size, Priority>::Intersection(Id id) : Node<Id, Size>{id} {}

  template <typename Id, typename Size, typename Priority>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  const dsm::priority_queue<Id, Priority>& Intersection<Id, Size, Priority>::queue() const {
    return m_queue;
  }

  template <typename Id, typename Size, typename Priority>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  bool Intersection<Id, Size, Priority>::isFull() const {
	return m_queue.size() >= this->capacity();
  }

  template <typename Id, typename Size, typename Priority>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  bool Intersection<Id, Size, Priority>::isFull(Id streetId) const {
  }

};  // namespace dsm

#endif
