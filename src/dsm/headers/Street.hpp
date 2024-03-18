/// @file       /src/dsm/headers/Street.hpp
/// @brief      Defines the Street class.
///
/// @details    This file contains the definition of the Street class.
///             The Street class represents a street in the network. It is templated by the
///             type of the street's id and the type of the street's capacity.
///             The street's id and capacity must be unsigned integral types.

#ifndef Street_hpp
#define Street_hpp

#include <optional>
#include <queue>
#include <type_traits>
#include <utility>
#include <stdexcept>
#include <cmath>
#include <numbers>

#include "Agent.hpp"
#include "Node.hpp"
#include "../utility/TypeTraits/is_numeric.hpp"
#include "../utility/queue.hpp"
#include "../utility/Logger.hpp"

namespace dsm {
  /// @brief The Street class represents a street in the network.
  /// @tparam Id, The type of the street's id. It must be an unsigned integral type.
  /// @tparam Size, The type of the street's capacity. It must be an unsigned integral type.
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  class Street {
  private:
    dsm::queue<Size> m_queue;
    std::pair<Id, Id> m_nodePair;
    double m_len;
    double m_maxSpeed;
    double m_angle;
    Id m_id;
    Size m_capacity;
    Size m_transportCapacity;
  public:
    Street() = delete;
    /// @brief Construct a new Street object starting from an existing street
    /// @details The new street has different id but same capacity, length, speed limit, and node pair as the
    ///          existing street.
    /// @param Street The existing street
    /// @param id The new street's id
    Street(Id id, const Street<Id, Size>&);
    /// @brief Construct a new Street object
    /// @param id The street's id
    /// @param nodePair The street's node pair
    Street(Id id, std::pair<Id, Id> nodePair);
    /// @brief Construct a new Street object
    /// @details The default capacity is 1, the default length is 1, and the default speed limit is
    ///          50 km/h, i.e. 13.8888888889 m/s.
    /// @param id The street's id
    /// @param capacity The street's capacity
    /// @param len The street's length
    /// @param nodePair The street's node pair
    Street(Id id, Size capacity, double len, std::pair<Id, Id> nodePair);
    /// @brief Construct a new Street object
    /// @details The default speed limit is 50 km/h, i.e. 13.8888888889 m/s.
    /// @param id The street's id
    /// @param capacity The street's capacity
    /// @param len The street's length
    /// @param maxSpeed The street's speed limit
    /// @param nodePair The street's node pair
    Street(Id id, Size capacity, double len, double maxSpeed, std::pair<Id, Id> nodePair);

    /// @brief Set the street's id
    /// @param id The street's id
    void setId(Id id);
    /// @brief Set the street's capacity
    /// @param capacity The street's capacity
    void setCapacity(Size capacity);
    /// @brief Set the street's transport capacity
    /// @details The transport capacity is the maximum number of agents that can traverse the street
    ///          in a time step.
    /// @param capacity The street's transport capacity
    void setTransportCapacity(Size capacity);
    /// @brief Set the street's length
    /// @param len The street's length
    /// @throw std::invalid_argument, If the length is negative
    void setLength(double len);
    /// @brief Set the street's queue
    /// @param queue The street's queue
    void setQueue(dsm::queue<Size> queue);
    /// @brief Set the street's node pair
    /// @param node1 The source node of the street
    /// @param node2 The destination node of the street
    void setNodePair(Id node1, Id node2);
    /// @brief Set the street's node pair
    /// @param node1 The source node of the street
    /// @param node2 The destination node of the street
    void setNodePair(const Node<Id, Size>& node1, const Node<Id, Size>& node2);
    /// @brief Set the street's node pair
    /// @param pair The street's node pair
    void setNodePair(std::pair<Id, Id> pair);
    /// @brief Set the street's speed limit
    /// @param speed The street's speed limit
    /// @throw std::invalid_argument, If the speed is negative
    void setMaxSpeed(double speed);
    /// @brief Set the street's angle
    /// @param srcNode The source node of the street
    /// @param dstNode The destination node of the street
    void setAngle(std::pair<double, double> srcNode, std::pair<double, double> dstNode);
    /// @brief Set the street's angle
    /// @param angle The street's angle
    /// @throw std::invalid_argument If the angle is negative or greater than 2 * pi
    void setAngle(double angle);

    /// @brief Get the street's id
    /// @return Id, The street's id
    Id id() const;
    /// @brief Get the street's capacity
    /// @return Size, The street's capacity
    Size capacity() const;
    /// @brief Get the street's transport capacity
    /// @details The transport capacity is the maximum number of agents that can traverse the street
    ///          in a time step.
    /// @return Size, The street's transport capacity
    Size transportCapacity() const;
    /// @brief Get the street's length
    /// @return double, The street's length
    double length() const;
    /// @brief Get the street's queue
    /// @return dsm::queue<Size>, The street's queue
    const dsm::queue<Size>& queue() const;
    /// @brief Get the street's node pair
    /// @return std::pair<Id, Id>, The street's node pair
    const std::pair<Id, Id>& nodePair() const;
    /// @brief Get the street's density
    /// @return double, The street's density
    double density() const;
    /// @brief Get the street's speed limit
    /// @return double, The street's speed limit
    double maxSpeed() const;
    /// @brief Get the street's angle
    /// @return double The street's angle
    double angle() const;
    /// @brief Add an agent to the street's queue
    /// @param agentId The id of the agent to add to the street's queue
    /// @throw std::runtime_error If the street's queue is full
    virtual void enqueue(Id agentId);
    /// @brief Remove an agent from the street's queue
    virtual std::optional<Id> dequeue();
    /// @brief Check if the street is a spire
    /// @return bool True if the street is a spire, false otherwise
    virtual bool isSpire() const { return false; };
  };

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  Street<Id, Size>::Street(Id id, const Street<Id, Size>& street)
      : m_nodePair{street.nodePair()},
        m_len{street.length()},
        m_maxSpeed{street.maxSpeed()},
        m_angle{street.angle()},
        m_id{id},
        m_capacity{street.capacity()},
        m_transportCapacity{street.transportCapacity()} {}

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  Street<Id, Size>::Street(Id index, std::pair<Id, Id> pair)
      : m_nodePair{std::move(pair)},
        m_len{1.},
        m_maxSpeed{13.8888888889},
        m_angle{0.},
        m_id{index},
        m_capacity{1},
        m_transportCapacity{std::numeric_limits<Size>::max()} {}

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  Street<Id, Size>::Street(Id id, Size capacity, double len, std::pair<Id, Id> nodePair)
      : m_nodePair{std::move(nodePair)},
        m_len{len},
        m_maxSpeed{13.8888888889},
        m_angle{0.},
        m_id{id},
        m_capacity{capacity},
        m_transportCapacity{std::numeric_limits<Size>::max()} {}

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  Street<Id, Size>::Street(
      Id id, Size capacity, double len, double maxSpeed, std::pair<Id, Id> nodePair)
      : m_nodePair{std::move(nodePair)},
        m_len{len},
        m_angle{0.},
        m_id{id},
        m_capacity{capacity},
        m_transportCapacity{std::numeric_limits<Size>::max()} {
    this->setMaxSpeed(maxSpeed);
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Street<Id, Size>::setId(Id id) {
    m_id = id;
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Street<Id, Size>::setCapacity(Size capacity) {
    m_capacity = capacity;
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Street<Id, Size>::setTransportCapacity(Size capacity) {
    m_transportCapacity = capacity;
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Street<Id, Size>::setLength(double len) {
    if (len < 0.) {
      throw std::invalid_argument(buildLog("The length of a street cannot be negative."));
    }
    m_len = len;
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Street<Id, Size>::setQueue(dsm::queue<Size> queue) {
    m_queue = std::move(queue);
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Street<Id, Size>::setNodePair(Id node1, Id node2) {
    m_nodePair = std::make_pair(node1, node2);
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Street<Id, Size>::setNodePair(const Node<Id, Size>& node1,
                                     const Node<Id, Size>& node2) {
    m_nodePair = std::make_pair(node1.id(), node2.id());
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Street<Id, Size>::setNodePair(std::pair<Id, Id> pair) {
    m_nodePair = std::move(pair);
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Street<Id, Size>::setMaxSpeed(double speed) {
    if (speed < 0.) {
      throw std::invalid_argument(
          buildLog("The maximum speed of a street cannot be negative."));
    }
    m_maxSpeed = speed;
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Street<Id, Size>::setAngle(std::pair<double, double> srcNode,
                                  std::pair<double, double> dstNode) {
    double angle{
        std::atan2(dstNode.second - srcNode.second, dstNode.first - srcNode.first)};
    if (angle < 0.) {
      angle += 2 * std::numbers::pi;
    }
    this->setAngle(angle);
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Street<Id, Size>::setAngle(double angle) {
    if (angle < 0. || angle > 2 * std::numbers::pi) {
      throw std::invalid_argument(
          buildLog("The angle of a street must be between 0 and 2 * pi."));
    }
    m_angle = angle;
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  Id Street<Id, Size>::id() const {
    return m_id;
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  Size Street<Id, Size>::capacity() const {
    return m_capacity;
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  Size Street<Id, Size>::transportCapacity() const {
    return m_transportCapacity;
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  double Street<Id, Size>::length() const {
    return m_len;
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  const dsm::queue<Size>& Street<Id, Size>::queue() const {
    return m_queue;
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  const std::pair<Id, Id>& Street<Id, Size>::nodePair() const {
    return m_nodePair;
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  double Street<Id, Size>::density() const {
    return static_cast<double>(m_queue.size()) / m_capacity;
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  double Street<Id, Size>::maxSpeed() const {
    return m_maxSpeed;
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  double Street<Id, Size>::angle() const {
    return m_angle;
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Street<Id, Size>::enqueue(Id agentId) {
    if (m_queue.size() == m_capacity) {
      throw std::runtime_error(buildLog("The street's queue is full."));
    }
    for (auto const& id : m_queue) {
      if (id == agentId) {
        throw std::runtime_error(buildLog("The agent is already on the street."));
      }
    }
    m_queue.push(agentId);
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  std::optional<Id> Street<Id, Size>::dequeue() {
    if (m_queue.empty()) {
      return std::nullopt;
    }
    Id id = m_queue.front();
    m_queue.pop();
    return id;
  }

  /// @brief The SpireStreet class represents a street which is able to count agent flows in both input and output.
  /// @tparam Id The type of the street's id
  /// @tparam Size The type of the street's capacity
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  class SpireStreet : public Street<Id, Size> {
  private:
    Size m_agentCounterIn;
    Size m_agentCounterOut;
  public:
    SpireStreet() = delete;
    /// @brief Construct a new SpireStreet object starting from an existing street
    /// @param id The street's id
    /// @param street The existing street
    SpireStreet(Id id, const Street<Id, Size>& street);
    /// @brief Construct a new SpireStreet object
    /// @param id The street's id
    /// @param capacity The street's capacity
    /// @param len The street's length
    /// @param nodePair The street's node pair
    SpireStreet(Id id, Size capacity, double len, std::pair<Id, Id> nodePair);
    /// @brief Construct a new SpireStreet object
    /// @param id The street's id
    /// @param capacity The street's capacity
    /// @param len The street's length
    /// @param maxSpeed The street's speed limit
    /// @param nodePair The street's node pair
    SpireStreet(Id id, Size capacity, double len, double maxSpeed, std::pair<Id, Id> nodePair);
    ~SpireStreet() = default;

    /// @brief Add an agent to the street's queue
    /// @param agentId The id of the agent to add to the street's queue
    /// @throw std::runtime_error If the street's queue is full
    void enqueue(Id agentId) override;

    /// @brief Get the input flow of the street
    /// @return Size The input flow of the street
    /// @details Once the input flow is retrieved, it is reset to 0 together with the output flow.
    Size inputFlow();
    /// @brief Get the output flow of the street
    /// @return Size The output flow of the street
    /// @details Once the output flow is retrieved, it is reset to 0 together with the input flow.
    Size outputFlow();
    /// @brief Get the mean flow of the street
    /// @return int The flow of the street, i.e. the difference between input and output flows
    /// @details Once the flow is retrieved, bothh the input and output flows are reset to 0.
    ///     Notice that this flow is positive iff the input flow is greater than the output flow.
    int meanFlow();
    /// @brief Remove an agent from the street's queue
    /// @return std::optional<Id> The id of the agent removed from the street's queue
    std::optional<Id> dequeue() override;
    /// @brief Check if the street is a spire
    /// @return bool True if the street is a spire, false otherwise
    bool isSpire() const override { return true; };
  };

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  SpireStreet<Id, Size>::SpireStreet(Id id, const Street<Id, Size>& street)
      : Street<Id, Size>(id, street), m_agentCounterIn{0}, m_agentCounterOut{0} {}

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  SpireStreet<Id, Size>::SpireStreet(Id id, Size capacity, double len, std::pair<Id, Id> nodePair)
      : Street<Id, Size>(id, capacity, len, nodePair), m_agentCounterIn{0}, m_agentCounterOut{0} {}

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  SpireStreet<Id, Size>::SpireStreet(
      Id id, Size capacity, double len, double maxSpeed, std::pair<Id, Id> nodePair)
      : Street<Id, Size>(id, capacity, len, maxSpeed, nodePair), m_agentCounterIn{0}, m_agentCounterOut{0} {}

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void SpireStreet<Id, Size>::enqueue(Id agentId) {
    Street<Id, Size>::enqueue(agentId);
    ++m_agentCounterIn;
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  Size SpireStreet<Id, Size>::inputFlow() {
    Size flow = m_agentCounterIn;
    m_agentCounterIn = 0;
    m_agentCounterOut = 0;
    return flow;
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  Size SpireStreet<Id, Size>::outputFlow() {
    Size flow = m_agentCounterOut;
    m_agentCounterIn = 0;
    m_agentCounterOut = 0;
    return flow;
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  int SpireStreet<Id, Size>::meanFlow() {
    int flow = static_cast<int>(m_agentCounterIn) - static_cast<int>(m_agentCounterOut);
    m_agentCounterIn = 0;
    m_agentCounterOut = 0; 
    return flow;
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  std::optional<Id> SpireStreet<Id, Size>::dequeue() {
    std::optional<Id> id = Street<Id, Size>::dequeue();
    if (id.has_value()) {
      ++m_agentCounterOut;
    }
    return id;
  }

};  // namespace dsm

#endif
