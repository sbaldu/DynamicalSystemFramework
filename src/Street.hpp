
#ifndef Street_hpp
#define Street_hpp

#include <optional>
#include <queue>
#include <type_traits>
#include <utility>
#include <stdexcept>
#include <string>

#include "Agent.hpp"
#include "Node.hpp"
#include "utility/TypeTraits/is_numeric.hpp"

namespace dmf {

  template <typename Id, typename Size>
  requires(std::unsigned_integral<Id>&& std::unsigned_integral<Size>) class Street {
  private:
    Id m_id;
    Size m_size;
    Size m_capacity;
    double m_len;
    std::pair<Id, Id> m_nodePair;
    std::queue<Size> m_queue;
    double m_maxSpeed;

  public:
    Street() = default;
    Street(Id index, Size capacity, double len);
    Street(Id index, Size capacity, double len, std::pair<Id, Id> nodePair);
    Street(Id index, Size capacity, double len, std::pair<Id, Id> nodePair, double maxSpeed);

    // Setters
    void setId(Id id);
    void setCapacity(Size capacity);
    void setLength(double len);
    void setQueue(std::queue<Size> queue);
    void setNodePair(Id node1, Id node2);
    void setNodePair(const Node<Id>& node1, const Node<Id>& node2);
    void setNodePair(std::pair<Id, Id> pair);
    void setMaxSpeed(double speed);

    // Getters
    Id id() const;
    Size size() const;
    Size capacity() const;
    double length() const;
    const std::queue<Size>& queue() const;
    const std::pair<Id, Id>& nodePair() const;
    double density() const;
    double maxSpeed() const;

    template <typename Weight>
    requires is_numeric_v<Weight>
    void enqueue(const Agent<Id, Weight>& agent);
    std::optional<Id> dequeue();
  };

  template <typename Id, typename Size>
  requires(std::unsigned_integral<Id>&& std::unsigned_integral<Size>) Street<Id, Size>::Street(
      Id index, Size capacity, double len)
      : m_id{index}, m_capacity{capacity}, m_len{len}, m_maxSpeed{30.} {}

  template <typename Id, typename Size>
  requires(std::unsigned_integral<Id>&& std::unsigned_integral<Size>) Street<Id, Size>::Street(
      Id index, Size capacity, double len, std::pair<Id, Id> nodePair)
      : m_id{index}, m_capacity{capacity}, m_len{len}, m_nodePair{std::move(nodePair)}, m_maxSpeed{30.} {}

  template <typename Id, typename Size>
  requires(std::unsigned_integral<Id>&& std::unsigned_integral<Size>) Street<Id, Size>::Street(
      Id index, Size capacity, double len, std::pair<Id, Id> nodePair, double maxSpeed)
      : m_id{index}, m_capacity{capacity}, m_len{len}, m_nodePair{std::move(nodePair)} {
    this->setMaxSpeed(maxSpeed);
  }

  // Setters
  template <typename Id, typename Size>
  requires(std::unsigned_integral<Id>&& std::unsigned_integral<Size>) void Street<Id, Size>::setId(Id id) {
    m_id = id;
  }
  template <typename Id, typename Size>
  requires(std::unsigned_integral<Id>&& std::unsigned_integral<Size>) void Street<Id, Size>::setCapacity(
      Size capacity) {
    m_capacity = capacity;
  }
  template <typename Id, typename Size>
  requires(std::unsigned_integral<Id>&& std::unsigned_integral<Size>) void Street<Id, Size>::setLength(
      double len) {
    m_len = len;
  }
  template <typename Id, typename Size>
  requires(std::unsigned_integral<Id>&& std::unsigned_integral<Size>) void Street<Id, Size>::setQueue(
      std::queue<Size> queue) {
    m_queue = std::move(queue);
  }
  template <typename Id, typename Size>
  requires(std::unsigned_integral<Id>&& std::unsigned_integral<Size>) void Street<Id, Size>::setNodePair(
      Id node1, Id node2) {
    m_nodePair = std::make_pair(node1, node2);
  }
  template <typename Id, typename Size>
  requires(std::unsigned_integral<Id>&& std::unsigned_integral<Size>) void Street<Id, Size>::setNodePair(
      const Node<Id>& node1, const Node<Id>& node2) {
    m_nodePair = std::make_pair(node1.id(), node2.id());
  }
  template <typename Id, typename Size>
  requires(std::unsigned_integral<Id>&& std::unsigned_integral<Size>) void Street<Id, Size>::setNodePair(
      std::pair<Id, Id> pair) {
    m_nodePair = std::move(pair);
  }
  template <typename Id, typename Size>
  requires(std::unsigned_integral<Id>&& std::unsigned_integral<Size>) void Street<Id, Size>::setMaxSpeed(
      double speed) {
    if (speed < 0.) {
      std::string errorMsg = "Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
                             "The maximum speed of a street cannot be negative.";
      throw std::invalid_argument(errorMsg);
    }
    m_maxSpeed = speed;
  }

  // Getters
  template <typename Id, typename Size>
  requires(std::unsigned_integral<Id>&& std::unsigned_integral<Size>) Id Street<Id, Size>::id()
  const { return m_id; }
  template <typename Id, typename Size>
  requires(std::unsigned_integral<Id>&& std::unsigned_integral<Size>) Size Street<Id, Size>::size()
  const { return m_size; }
  template <typename Id, typename Size>
  requires(std::unsigned_integral<Id>&& std::unsigned_integral<Size>) Size Street<Id, Size>::capacity()
  const { return m_capacity; }
  template <typename Id, typename Size>
  requires(
      std::unsigned_integral<Id>&& std::unsigned_integral<Size>) double Street<Id, Size>::length() const {
    return m_len;
  }
  template <typename Id, typename Size>
  requires(std::unsigned_integral<Id>&& std::unsigned_integral<Size>)
      const std::queue<Size>& Street<Id, Size>::queue() const {
    return m_queue;
  }
  template <typename Id, typename Size>
  requires(std::unsigned_integral<Id>&& std::unsigned_integral<Size>)
      const std::pair<Id, Id>& Street<Id, Size>::nodePair() const {
    return m_nodePair;
  }
  template <typename Id, typename Size>
  requires(
      std::unsigned_integral<Id>&& std::unsigned_integral<Size>) double Street<Id, Size>::density() const {
    return static_cast<double>(m_size) / m_capacity;
  }
  template <typename Id, typename Size>
  requires(
      std::unsigned_integral<Id>&& std::unsigned_integral<Size>) double Street<Id, Size>::maxSpeed() const {
    return m_maxSpeed;
  }

  template <typename Id, typename Size>
  requires(std::unsigned_integral<Id>&& std::unsigned_integral<Size>) template <typename Weight>
  requires is_numeric_v<Weight>
  void Street<Id, Size>::enqueue(const Agent<Id, Weight>& agent) {
    if (m_size < m_capacity) {
      m_queue.push(agent.index());
      ++m_size;
    }
  }

  template <typename Id, typename Size>
  requires(std::unsigned_integral<Id>&& std::unsigned_integral<Size>)
      std::optional<Id> Street<Id, Size>::dequeue() {
    if (m_size > 0) {
      Id res{m_queue.front()};
      m_queue.pop();
      --m_size;

      return res;
    }

    return std::nullopt;
  }

};  // namespace dmf

#endif
