/// @file       /src/dsm/headers/Dynamics.hpp
/// @brief      Defines the Dynamics class.
///
/// @details    This file contains the definition of the Dynamics class.
///             The Dynamics class represents the dynamics of the network. It is templated by the type
///             of the graph's id and the type of the graph's capacity.
///             The graph's id and capacity must be unsigned integral types.

#ifndef dynamics_hpp
#define dynamics_hpp

#include <algorithm>
#include <concepts>
#include <vector>
#include <random>
#include <span>
#include <string>

#include "Agent.hpp"
#include "Itinerary.hpp"
#include "Graph.hpp"
#include "../utility/TypeTraits/is_agent.hpp"
#include "../utility/TypeTraits/is_itinerary.hpp"

namespace dsm {

  using TimePoint = long long unsigned int;

  /// @brief The Dynamics class represents the dynamics of the network.
  /// @tparam Id, The type of the graph's id. It must be an unsigned integral type.
  /// @tparam Size, The type of the graph's capacity. It must be an unsigned integral type.
  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> && is_numeric_v<Delay>
  class Dynamics {
  private:
    std::vector<std::unique_ptr<Itinerary<Id>>> m_itineraries;
    std::vector<std::unique_ptr<Agent<Id, Size, Delay>>> m_agents;
    TimePoint m_time;
    std::unique_ptr<Graph<Id, Size>> m_graph;
    double m_temperature;
    double m_minSpeedRateo;
    std::mt19937_64 m_generator{std::random_device{}()};

  public:
    Dynamics() = delete;
    /// @brief Construct a new Dynamics object
    /// @param graph, The graph representing the network
    Dynamics(const Graph<Id, Size>& graph);

    /// @brief Set the itineraries
    /// @param itineraries, The itineraries
    void setItineraries(std::span<Itinerary<Id>> itineraries);
    /// @brief Set the seed for the graph's random number generator
    /// @param seed, The seed
    void setSeed(unsigned int seed);
    /// @brief Set the minim speed rateo, i.e. the minim speed with respect to the speed limit
    /// @param minSpeedRateo The minim speed rateo
    /// @throw std::invalid_argument, If the minim speed rateo is not between 0 and 1
    void setMinSpeedRateo(double minSpeedRateo);
    /// @brief Update the paths of the itineraries based on the actual travel times
    void updatePaths();  //TODO: implement

    /// @brief Get the graph
    /// @return const Graph<Id, Size>&, The graph
    const Graph<Id, Size>& graph() const;
    /// @brief Get the itineraries
    /// @return const std::vector<Itinerary<Id>>&, The itineraries
    const std::vector<std::unique_ptr<Itinerary<Id>>>& itineraries() const;
    /// @brief Get the agents
    /// @return const std::vector<Agent<Id>>&, The agents
    const std::vector<std::unique_ptr<Agent<Id, Size, Delay>>>& agents() const;
    /// @brief Get the time
    /// @return TimePoint, The time
    TimePoint time() const;

    /// @brief Add an agent to the simulation
    /// @param agent, The agent
    void addAgent(const Agent<Id, Size, Delay>& agent);
    /// @brief Add an agent to the simulation
    /// @param agent, Unique pointer to the agent
    void addAgent(std::unique_ptr<Agent<Id, Size, Delay>> agent);
    /// @brief Add a pack of agents to the simulation
    /// @param agents, Parameter pack of agents
    template <typename... Tn>
      requires(is_agent_v<Tn> && ...)
    void addAgents(Tn... agents);
    /// @brief Add a pack of agents to the simulation
    /// @param agent, An agent
    /// @param agents, Parameter pack of agents
    template <typename T1, typename... Tn>
      requires(is_agent_v<T1> && (is_agent_v<Tn> && ...))
    void addAgents(T1 agent, Tn... agents);
    /// @brief Add a set of agents to the simulation
    /// @param agents, Generic container of agents, represented by an std::span
    void addAgents(std::span<Agent<Id, Size, Delay>> agents);

    /// @brief Remove an agent from the simulation
    /// @param agentId, the index of the agent to remove
    void removeAgent(Size agentId);
    template <typename T1, typename... Tn>
      requires(std::is_convertible_v<T1, Size> && (std::is_convertible_v<Tn, Size> && ...))
    /// @brief Remove a pack of agents from the simulation
    /// @param id, the index of the first agent to remove
    /// @param ids, the pack of indexes of the agents to remove
    void removeAgents(T1 id, Tn... ids);

    void addRandomAgents(uint nAgents);  // TODO: implement

    /// @brief Add an itinerary
    /// @param itinerary, The itinerary
    void addItinerary(const Itinerary<Id>& itinerary);
    /// @brief Add an itinerary
    /// @param itinerary, Unique pointer to the itinerary
    void addItinerary(std::unique_ptr<Itinerary<Id>> itinerary);
    template <typename... Tn>
      requires(is_itinerary_v<Tn> && ...)
    void addItineraries(Tn... itineraries);
    template <typename T1, typename... Tn>
      requires(is_itinerary_v<T1> && (is_itinerary_v<Tn> && ...))
    void addItineraries(T1 itinerary, Tn... itineraries);

    void addItineraries(std::span<Itinerary<Id>> itineraries);

    /// @brief Reset the simulation time
    void resetTime();

    template <typename F, typename... Tn>
      requires std::is_invocable_v<F, Tn...>
    void evolve(F f, Tn... args);
  };

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  Dynamics<Id, Size, Delay>::Dynamics(const Graph<Id, Size>& graph)
      : m_graph{std::make_unique<Graph<Id, Size>>(graph)} {}

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Dynamics<Id, Size, Delay>::setItineraries(std::span<Itinerary<Id>> itineraries) {
    std::ranges::for_each(itineraries, [this](const auto& itinerary) {
      this->m_itineraries.insert(std::make_unique<Itinerary<Id>>(itinerary));
    });
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Dynamics<Id, Size, Delay>::setSeed(unsigned int seed) {
    m_generator.seed(seed);
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Dynamics<Id, Size, Delay>::setMinSpeedRateo(double minSpeedRateo) {
    if (minSpeedRateo < 0. || minSpeedRateo > 1.) {
      std::string errorMsg{"Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
                           "The minim speed rateo must be between 0 and 1"};
      throw std::invalid_argument(errorMsg);
    }
    m_minSpeedRateo = minSpeedRateo;
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  const Graph<Id, Size>& Dynamics<Id, Size, Delay>::graph() const {
    return *m_graph;
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  const std::vector<std::unique_ptr<Itinerary<Id>>>& Dynamics<Id, Size, Delay>::itineraries() const {
    return m_itineraries;
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  const std::vector<std::unique_ptr<Agent<Id, Size, Delay>>>& Dynamics<Id, Size, Delay>::agents() const {
    return m_agents;
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  TimePoint Dynamics<Id, Size, Delay>::time() const {
    return m_time;
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Dynamics<Id, Size, Delay>::addAgent(const Agent<Id, Size, Delay>& agent) {
    m_agents.insert(std::make_unique<Agent<Id, Size, Delay>>(agent));
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Dynamics<Id, Size, Delay>::addAgent(std::unique_ptr<Agent<Id, Size, Delay>> agent) {
    m_agents.insert(std::move(agent));
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  template <typename... Tn>
    requires(is_agent_v<Tn> && ...)
  void Dynamics<Id, Size, Delay>::addAgents(Tn... agents) {}

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  template <typename T1, typename... Tn>
    requires(is_agent_v<T1> && (is_agent_v<Tn> && ...))
  void Dynamics<Id, Size, Delay>::addAgents(T1 agent, Tn... agents) {
    addAgent(agent);
    addAgents(agents...);
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Dynamics<Id, Size, Delay>::addAgents(std::span<Agent<Id, Size, Delay>> agents) {
    std::ranges::for_each(
        agents, [this](const auto& agent) -> void { this->m_agents.push_back(std::make_unique(agent)); });
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Dynamics<Id, Size, Delay>::removeAgent(Size agentId) {
    auto agentIt{std::find_if(
        m_agents.begin(), m_agents.end(), [agentId](auto agent) { return agent->index() == agentId; })};
    if (agentIt != m_agents.end()) {
      m_agents.erase(agentIt);
    }
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  template <typename T1, typename... Tn>
    requires(std::is_convertible_v<T1, Size> && (std::is_convertible_v<Tn, Size> && ...))
  void Dynamics<Id, Size, Delay>::removeAgents(T1 id, Tn... ids) {
    removeAgent(id);
    removeAgents(ids...);
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Dynamics<Id, Size, Delay>::addItinerary(const Itinerary<Id>& itinerary) {
    m_itineraries.insert(std::make_unique<Itinerary<Id>>(itinerary));
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Dynamics<Id, Size, Delay>::addItinerary(std::unique_ptr<Itinerary<Id>> itinerary) {
    m_itineraries.insert(std::move(itinerary));
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  template <typename... Tn>
    requires(is_itinerary_v<Tn> && ...)
  void Dynamics<Id, Size, Delay>::addItineraries(Tn... itineraries) {
    (this->addItinerary(itineraries), ...);
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Dynamics<Id, Size, Delay>::addItineraries(std::span<Itinerary<Id>> itineraries) {
    std::ranges::for_each(itineraries, [this](const auto& itinerary) -> void {
      this->m_itineraries.push_back(std::make_unique<Itinerary<Id>>(itinerary));
    });
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Dynamics<Id, Size, Delay>::resetTime() {
    m_time = 0;
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  template <typename F, typename... Tn>
    requires std::is_invocable_v<F, Tn...>
  void Dynamics<Id, Size, Delay>::evolve(F f, Tn... args) {
    f(args...);
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>)
  class FirstOrderDynamics : public Dynamics<Id, Size, Delay> {
  private:
    std::vector<std::unique_ptr<Agent<Id, Size, Delay>>> m_agents;

  public:
    FirstOrderDynamics() = delete;
    /// @brief Construct a new First Order Dynamics object
    /// @param graph, The graph representing the network
    FirstOrderDynamics(const Graph<Id, Size>& graph);
    /// @brief Set the speed of an agent
    /// @param agentId, The index of the agent
    /// @throw std::invalid_argument, If the agent is not found
    void setAgentSpeed(Size agentId);
    void setSpeed();
  };

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>)
  FirstOrderDynamics<Id, Size, Delay>::FirstOrderDynamics(const Graph<Id, Size>& graph)
      : Dynamics<Id, Size, Delay>(graph) {}

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void FirstOrderDynamics<Id, Size, Delay>::setAgentSpeed(Size agentId) {
    auto agentIt{std::find_if(
        m_agents.begin(), m_agents.end(), [agentId](auto agent) { return agent->index() == agentId; })};
    if (agentIt == m_agents.end()) {
      std::string errorMsg{"Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
                           "Agent " + std::to_string(agentId) + " not found"};
      throw std::invalid_argument(errorMsg);
    }
    auto& agent{*agentIt};
    auto& street{this->m_graph->street(agent->position())};
    double speed{street->maxSpeed() * (1. - this->m_minSpeedRateo * street->density())};
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void FirstOrderDynamics<Id, Size, Delay>::setSpeed() {}

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  class SecondOrderDynamics : public Dynamics<Id, Size, double> {
  public:
    void setAgentSpeed(Size agentId);
    void setSpeed();
  };

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void SecondOrderDynamics<Id, Size>::setAgentSpeed(Size agentId) {}

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void SecondOrderDynamics<Id, Size>::setSpeed() {}

};  // namespace dsm

#endif
