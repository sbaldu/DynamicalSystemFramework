
#ifndef Street_hpp
#define Street_hpp

#include <queue>
#include <type_traits>
#include <utility>

namespace dmf {

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  class Street {
  private:
    Id m_id;
    Size m_size;
    Size m_capacity;
    double m_len;
    std::pair<Id, Id> m_nodePair;
    std::queue<Size> m_queue;

  public:
    Street() = default;
    Street(Id index, Size capacity, double len);
    Street(Id index, Size size, Size capacity, double len);

    // Setters
    void setCapacity(Size capacity);
    void setLength(double len);
    void setQueue(std::queue<Size> queue);

    // Getters
    Id id() const;
    Size size() const;
    Size capacity() const;
    double length() const;
    const std::queue<Size>& queue() const;
    const std::pair<Id, Id>& nodePair() const;
  };

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  Street<Id, Size>::Street(Id index, Size capacity, double len)
      : m_id{index}, m_capacity{capacity}, m_len{len} {}

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  Street<Id, Size>::Street(Id index, Size size, Size capacity, double len)
      : m_id{index}, m_size{size}, m_capacity{capacity}, m_len{len} {}

  // Setters
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Street<Id, Size>::setCapacity(Size capacity) {
    m_capacity = capacity;
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Street<Id, Size>::setLength(double len) {
    m_len = len;
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Street<Id, Size>::setQueue(std::queue<Size> queue) {
    m_queue = std::move(queue);
  }

  // Getters
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  Id Street<Id, Size>::id() const {
    return m_id;
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  Size Street<Id, Size>::size() const {
    return m_size;
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  Size Street<Id, Size>::capacity() const {
    return m_capacity;
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  double Street<Id, Size>::length() const {
    return m_len;
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  const std::queue<Size>& Street<Id, Size>::queue() const {
    return m_queue;
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  const std::pair<Id, Id>& Street<Id, Size>::nodePair() const {
    return m_nodePair;
  }
};  // namespace dmf

#endif
