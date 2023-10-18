
#ifndef Node_hpp
#define Node_hpp

#include <functional>
#include <queue>
#include <utility>

namespace dmf {

  template <typename Id>
  requires std::unsigned_integral<Id>
  class Node {
  private:
    Id m_id;
    std::pair<double, double> m_coords;
    std::queue<Id> m_queue;

  public:
    Node() = default;
    explicit Node(Id id);
    Node(Id id, std::pair<double, double> coords);
    Node(Id id, std::pair<double, double> coords, std::queue<Id> queue);

    // Setters
    void setCoords(std::pair<double, double> coords);
    void setQueue(std::queue<Id> queue);

    // Getters
    Id id() const;
    const std::pair<double, double>& coords() const;
    const std::queue<Id>& queue() const;
  };

  template <typename Id>
  requires std::unsigned_integral<Id> Node<Id>::Node(Id id) : m_id{id} {}

  template <typename Id>
  requires std::unsigned_integral<Id> Node<Id>::Node(Id id, std::pair<double, double> coords)
      : m_id{id}, m_coords{std::move(coords)} {}

  template <typename Id>
  requires std::unsigned_integral<Id> Node<Id>::Node(Id id,
                                                     std::pair<double, double> coords,
                                                     std::queue<Id> queue)
      : m_id{id}, m_coords{std::move(coords)}, m_queue{std::move(queue)} {}

  template <typename Id>
  requires std::unsigned_integral<Id> Id Node<Id>::id()
  const { return m_id; }

  template <typename Id>
  requires std::unsigned_integral<Id>
  void Node<Id>::setCoords(std::pair<double, double> coords) { m_coords = std::move(coords); }

  template <typename Id>
  requires std::unsigned_integral<Id>
  void Node<Id>::setQueue(std::queue<Id> queue) { m_queue = std::move(queue); }

  template <typename Id>
  requires std::unsigned_integral<Id>
  const std::pair<double, double>& Node<Id>::coords() const { return m_coords; }

  template <typename Id>
  requires std::unsigned_integral<Id>
  const std::queue<Id>& Node<Id>::queue() const { return m_queue; }

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
