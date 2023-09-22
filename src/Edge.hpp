
#ifndef Edge_hpp
#define Edge_hpp

#include <queue>

namespace dmf {

  template <typename Id, typename Size>
  class Edge {
  private:
    Id m_index;
    Size m_size;
    Size m_capacity;
    std::queue<Size> m_queue;

  public:
    Edge() = default;
    Edge(Id index, Size capacity);
    Edge(Id index, Size size, Size capacity);

    // Setters
    void setIndex(Id index);
    void setCapacity(Size capacity);
    void setQueue(std::queue<Size> queue);

    // Getters
    Id index() const;
    Size size() const;
    Size capacity() const;
    const std::queue<Size>& queue() const;
  };

  template <typename Id, typename Size>
  Edge<Id, Size>::Edge(Id index, Size capacity) : m_index{index}, m_capacity{capacity} {}

  template <typename Id, typename Size>
  Edge<Id, Size>::Edge(Id index, Size size, Size capacity)
      : m_index{index}, m_size{size}, m_capacity{capacity} {}

  // Setters
  template <typename Id, typename Size>
  void Edge<Id, Size>::setIndex(Id index) {
    m_index = index;
  }
  template <typename Id, typename Size>
  void Edge<Id, Size>::setCapacity(Size capacity) {
    m_capacity = capacity;
  }
  template <typename Id, typename Size>
  void Edge<Id, Size>::setQueue(std::queue<Size> queue) {
    m_queue = std::move(queue);
  }

  // Getters
  template <typename Id, typename Size>
  Id Edge<Id, Size>::index() const {
    return m_index;
  }
  template <typename Id, typename Size>
  Size Edge<Id, Size>::size() const {
    return m_size;
  }
  template <typename Id, typename Size>
  Size Edge<Id, Size>::capacity() const {
    return m_capacity;
  }
  template <typename Id, typename Size>
  const std::queue<Size>& Edge<Id, Size>::queue() const {
    return m_queue;
  }
};  // namespace dmf

#endif
