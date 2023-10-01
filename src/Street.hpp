
#ifndef Street_hpp
#define Street_hpp

#include <queue>

#include "CXXGraph.hpp"

namespace dmf {

  template <typename Id, template <typename Id_> typename Container, typename Priority, typename Size>
  class Street : public CXXGraph::Edge<std::priority_queue<Id, Container<Id>, Priority>> {
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

	virtual bool isOneWay();
	virtual bool isTwoWay();
  };

  template <typename Id, template <typename Id_> typename Container, typename Priority, typename Size>
  Street<Id, Container, Priority, Size>::Street(Id index, Size capacity) : m_capacity{capacity} {}

  template <typename Id, template <typename Id_> typename Container, typename Priority, typename Size>
  Street<Id, Container, Priority, Size>::Street(Id index, Size size, Size capacity)
      : m_size{size}, m_capacity{capacity} {}

  // Setters
  template <typename Id, template <typename Id_> typename Container, typename Priority, typename Size>
  void Street<Id, Container, Priority, Size>::setCapacity(Size capacity) {
    m_capacity = capacity;
  }
  template <typename Id, template <typename Id_> typename Container, typename Priority, typename Size>
  void Street<Id, Container, Priority, Size>::setQueue(std::queue<Size> queue) {
    m_queue = std::move(queue);
  }

  // Getters
  template <typename Id, template <typename Id_> typename Container, typename Priority, typename Size>
  Size Street<Id, Container, Priority, Size>::size() const {
    return m_size;
  }
  template <typename Id, template <typename Id_> typename Container, typename Priority, typename Size>
  Size Street<Id, Container, Priority, Size>::capacity() const {
    return m_capacity;
  }
  template <typename Id, template <typename Id_> typename Container, typename Priority, typename Size>
  const std::queue<Size>& Street<Id, Container, Priority, Size>::queue() const {
    return m_queue;
  }
};  // namespace dmf

#endif
