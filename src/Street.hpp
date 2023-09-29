
#ifndef Street_hpp
#define Street_hpp

#include <queue>

#include "CXXGraph.hpp"

namespace dmf {

  template <typename Id, typename Size>
  class Street : public CXXGraph::Edge<Id> {
  private:
    Size m_size;
    Size m_capacity;
    std::queue<Size> m_queue;

  public:
    Street() = default;
    Street(Id index, Size capacity);
    Street(Id index, Size size, Size capacity);

    // Setters
    void setCapacity(Size capacity);
    void setQueue(std::queue<Size> queue);

    // Getters
    Size size() const;
    Size capacity() const;
    const std::queue<Size>& queue() const;
  };

  template <typename Id, typename Size>
  Street<Id, Size>::Street(Id index, Size capacity) : m_capacity{capacity} {}

  template <typename Id, typename Size>
  Street<Id, Size>::Street(Id index, Size size, Size capacity)
      : m_size{size}, m_capacity{capacity} {}

  // Setters
  template <typename Id, typename Size>
  void Street<Id, Size>::setCapacity(Size capacity) {
    m_capacity = capacity;
  }
  template <typename Id, typename Size>
  void Street<Id, Size>::setQueue(std::queue<Size> queue) {
    m_queue = std::move(queue);
  }

  // Getters
  template <typename Id, typename Size>
  Size Street<Id, Size>::size() const {
    return m_size;
  }
  template <typename Id, typename Size>
  Size Street<Id, Size>::capacity() const {
    return m_capacity;
  }
  template <typename Id, typename Size>
  const std::queue<Size>& Street<Id, Size>::queue() const {
    return m_queue;
  }
};  // namespace dmf

#endif
