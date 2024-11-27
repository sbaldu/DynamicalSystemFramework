/// @file       /src/dsm/headers/Node.hpp
/// @brief      Defines the Node class.
///
/// @details    This file contains the definition of the Node class.
///             The Node class represents a node in the network. It is templated by the type
///             of the node's id, which must be an unsigned integral type.
///             The derived classes are:
///             - Intersection: represents an intersection node with a map of agents
///               - TrafficLight: represents a traffic light intersection node
///             - Roundabout: represents a roundabout node with a queue of agents

#pragma once

#include <functional>
#include <utility>
#include <stdexcept>
#include <optional>
#include <set>
#include <map>
#include <format>
#include <cassert>

#include "../utility/Logger.hpp"
#include "../utility/queue.hpp"
#include "../utility/Typedef.hpp"

namespace dsm {
  /// @brief The Node class represents the concept of a node in the network.
  /// @tparam Id The type of the node's id
  /// @tparam Size The type of the node's capacity
  class Node {
  protected:
    Id m_id;
    std::optional<std::pair<double, double>> m_coords;
    Size m_capacity;
    Size m_transportCapacity;

  public:
    /// @brief Construct a new Node object with capacity 1
    /// @param id The node's id
    explicit Node(Id id) : m_id{id}, m_capacity{1}, m_transportCapacity{1} {}
    /// @brief Construct a new Node object with capacity 1
    /// @param id The node's id
    /// @param coords A std::pair containing the node's coordinates (lat, lon)
    Node(Id id, std::pair<double, double> coords)
        : m_id{id}, m_coords{std::move(coords)}, m_capacity{1}, m_transportCapacity{1} {}

    Node(Node const& other)
        : m_id{other.m_id},
          m_coords{other.m_coords},
          m_capacity{other.m_capacity},
          m_transportCapacity{other.m_transportCapacity} {};
    virtual ~Node() = default;

    /// @brief Set the node's id
    /// @param id The node's id
    void setId(Id id) { m_id = id; }
    /// @brief Set the node's coordinates
    /// @param coords A std::pair containing the node's coordinates (lat, lon)
    void setCoords(std::pair<double, double> coords) { m_coords = std::move(coords); }
    /// @brief Set the node's capacity
    /// @param capacity The node's capacity
    virtual void setCapacity(Size capacity) { m_capacity = capacity; }
    /// @brief Set the node's transport capacity
    /// @param capacity The node's transport capacity
    virtual void setTransportCapacity(Size capacity) { m_transportCapacity = capacity; }
    /// @brief Get the node's id
    /// @return Id The node's id
    Id id() const { return m_id; }
    /// @brief Get the node's coordinates
    /// @return std::optional<std::pair<double, double>> A std::pair containing the node's coordinates
    const std::optional<std::pair<double, double>>& coords() const { return m_coords; }
    /// @brief Get the node's capacity
    /// @return Size The node's capacity
    Size capacity() const { return m_capacity; }
    /// @brief Get the node's transport capacity
    /// @return Size The node's transport capacity
    Size transportCapacity() const { return m_transportCapacity; }

    virtual double density() const = 0;
    virtual bool isFull() const = 0;

    virtual bool isIntersection() const noexcept { return false; }
    virtual bool isTrafficLight() const noexcept { return false; }
    virtual bool isRoundabout() const noexcept { return false; }
  };

  /// @brief The Intersection class represents a node in the network.
  /// @tparam Id The type of the node's id. It must be an unsigned integral type.
  class Intersection : public Node {
  protected:
    std::multimap<int16_t, Id> m_agents;
    std::set<Id>
        m_streetPriorities;  // A set containing the street ids that have priority - like main roads
    Size m_agentCounter;

  public:
    Intersection() = default;
    /// @brief Construct a new Intersection object
    /// @param id The node's id
    explicit Intersection(Id id) : Node{id} {};
    /// @brief Construct a new Intersection object
    /// @param id The node's id
    /// @param coords A std::pair containing the node's coordinates
    Intersection(Id id, std::pair<double, double> coords) : Node{id, coords} {};

    Intersection(Node const& node) : Node{node} {};

    virtual ~Intersection() = default;

    /// @brief Set the node's capacity
    /// @param capacity The node's capacity
    /// @throws std::runtime_error if the capacity is smaller than the current queue size
    void setCapacity(Size capacity) override;

    /// @brief Put an agent in the node
    /// @param agent A std::pair containing the agent's angle difference and id
    /// @details The agent's angle difference is used to order the agents in the node.
    ///          The agent with the smallest angle difference is the first one to be
    ///          removed from the node.
    /// @throws std::runtime_error if the node is full
    void addAgent(double angle, Id agentId);
    /// @brief Put an agent in the node
    /// @param agentId The agent's id
    /// @details The agent's angle difference is used to order the agents in the node.
    ///          The agent with the smallest angle difference is the first one to be
    ///          removed from the node.
    /// @throws std::runtime_error if the node is full
    void addAgent(Id agentId);
    /// @brief Removes an agent from the node
    /// @param agentId The agent's id
    void removeAgent(Id agentId);
    /// @brief Set the node streets with priority
    /// @param streetPriorities A std::set containing the node's street priorities
    void setStreetPriorities(std::set<Id> streetPriorities) {
      m_streetPriorities = std::move(streetPriorities);
    }
    /// @brief Add a street to the node street priorities
    /// @param streetId The street's id
    void addStreetPriority(Id streetId) { m_streetPriorities.emplace(streetId); }
    /// @brief Returns the node's density
    /// @return double The node's density
    double density() const override {
      return static_cast<double>(m_agents.size()) / m_capacity;
    }
    /// @brief Returns true if the node is full
    /// @return bool True if the node is full
    bool isFull() const override { return m_agents.size() == this->m_capacity; }

    /// @brief Get the node's street priorities
    /// @details This function returns a std::set containing the node's street priorities.
    ///        If a street has priority, it means that the agents that are on that street
    ///        have priority over the agents that are on the other streets.
    /// @return std::set<Id> A std::set containing the node's street priorities
    virtual const std::set<Id>& streetPriorities() const { return m_streetPriorities; };
    /// @brief Get the node's agent ids
    /// @return std::set<Id> A std::set containing the node's agent ids
    const std::multimap<int16_t, Id>& agents() { return m_agents; };
    /// @brief Returns the number of agents that have passed through the node
    /// @return Size The number of agents that have passed through the node
    /// @details This function returns the number of agents that have passed through the node
    ///          since the last time this function was called. It also resets the counter.
    Size agentCounter();

    bool isIntersection() const noexcept final { return true; }
  };

  class TrafficLightCycle {
  private:
    Delay m_greenTime;
    Delay m_phase;

  public:
    /// @brief Construct a new TrafficLightCycle object
    /// @param greenTime Delay, the green time
    /// @param phase Delay, the phase
    TrafficLightCycle(Delay greenTime, Delay phase)
        : m_greenTime{greenTime}, m_phase{phase} {}

    inline Delay greenTime() const { return m_greenTime; }
    inline Delay phase() const { return m_phase; }
    /// @brief Returns true if the traffic light is green
    /// @param cycleTime Delay, the total time of a red-green cycle
    /// @param counter Delay, the current counter
    /// @return true if counter < m_phase || (counter >= m_phase + m_greenTime && counter < cycleTime)
    bool isGreen(Delay const cycleTime, Delay const counter) const;
  };

  class TrafficLight : public Intersection {
  private:
    std::unordered_map<Id, std::vector<TrafficLightCycle>> m_cycles;
    Delay m_cycleTime;  // The total time of a red-green cycle
    Delay m_counter;

  public:
    /// @brief Construct a new TrafficLight object
    /// @param id The node's id
    /// @param cycleTime The node's cycle time
    TrafficLight(Id id, Delay cycleTime)
        : Intersection{id}, m_cycleTime{cycleTime}, m_counter{0} {}

    TrafficLight(Node const& node, Delay cycleTime)
        : Intersection{node}, m_cycleTime{cycleTime}, m_counter{0} {}

    inline TrafficLight& operator++() {
      m_counter = (m_counter + 1) % m_cycleTime;
      return *this;
    }

    inline Delay cycleTime() const { return m_cycleTime; }

    void setCycle(Id const streetId, Direction direction, TrafficLightCycle const& cycle);

    void moveCycle(Id const oldStreetId, Id const newStreetId);

    inline std::unordered_map<Id, std::vector<TrafficLightCycle>> const& cycles() const {
      return m_cycles;
    }

    bool isGreen(Id const streetId, Direction direction) const;

    inline bool isTrafficLight() const noexcept final { return true; }
  };

  /// @brief The Roundabout class represents a roundabout node in the network.
  /// @tparam Id The type of the node's id
  /// @tparam Size The type of the node's capacity
  class Roundabout : public Node {
  protected:
    dsm::queue<Id> m_agents;

  public:
    Roundabout() = default;
    /// @brief Construct a new Roundabout object
    /// @param id The node's id
    explicit Roundabout(Id id) : Node{id} {};
    /// @brief Construct a new Roundabout object
    /// @param id The node's id
    /// @param coords A std::pair containing the node's coordinates
    Roundabout(Id id, std::pair<double, double> coords) : Node{id, coords} {};
    /// @brief Construct a new Roundabout object
    /// @param node An Intersection object
    Roundabout(const Node& node);

    virtual ~Roundabout() = default;

    /// @brief Put an agent in the node
    /// @param agentId The agent's id
    /// @throws std::runtime_error if the node is full
    void enqueue(Id agentId);
    /// @brief Removes the first agent from the node
    /// @return Id The agent's id
    Id dequeue();
    /// @brief Get the node's queue
    /// @return dsm::queue<Id> The node's queue
    const dsm::queue<Id>& agents() const { return m_agents; }
    /// @brief Returns the node's density
    /// @return double The node's density
    double density() const override {
      return static_cast<double>(m_agents.size()) / m_capacity;
    }
    /// @brief Returns true if the node is full
    /// @return bool True if the node is full
    bool isFull() const override { return m_agents.size() == this->m_capacity; }
    /// @brief Returns true if the node is a roundabout
    /// @return bool True if the node is a roundabout
    bool isRoundabout() const noexcept override { return true; }
  };

};  // namespace dsm
