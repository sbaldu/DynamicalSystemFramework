/// @file       /src/dsm/headers/Dynamics.hpp
/// @brief      Defines the Dynamics class.
///
/// @details    This file contains the definition of the Dynamics class.
///             The Dynamics class represents the dynamics of the network. It is templated by the type
///             of the graph's id and the type of the graph's capacity.
///             The graph's id and capacity must be unsigned integral types.

#ifndef dynamics_hpp
#define dynamics_hpp

#include <vector>
#include <random>

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
  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  class Dynamics {
  private:
    std::vector<std::unique_ptr<Itinerary<Id>>> m_itineraries;
    std::vector<std::unique_ptr<Agent<Id>>> m_agents;
    TimePoint m_time;
    std::unique_ptr<Graph<Id, Size>> m_graph;
    double m_temperature;
    std::mt19937_64 m_generator{std::random_device{}()};

  public:
    Dynamics() = delete;
    /// @brief Construct a new Dynamics object
    /// @param graph The graph representing the network
    Dynamics(const Graph<Id, Size>& graph);

    /// @brief Set the itineraries
    /// @param itineraries The itineraries
    void setItineraries(const std::vector<Itinerary<Id>>& itineraries);
    /// @brief Set the agents
    /// @param agents The agents
    void setAgents(const std::vector<Agent<Id>>& agents);
    /// @brief Set the seed for the graph's random number generator
    /// @param seed The seed
    void setSeed(uint seed);
    /// @brief Update the paths of the itineraries based on the actual travel times
    void updatePaths();  //TODO: implement

    /// @brief Get the graph
    /// @return const Graph<Id, Size>& The graph
    const Graph<Id, Size>& graph() const;
    /// @brief Get the itineraries
    /// @return const std::vector<Itinerary<Id>>&, The itineraries
    const std::vector<std::unique_ptr<Itinerary<Id>>>& itineraries() const;
    /// @brief Get the agents
    /// @return const std::vector<Agent<Id>>&, The agents
    const std::vector<std::unique_ptr<Agent<Id>>>& agents() const;
    /// @brief Get the time
    /// @return TimePoint The time
    TimePoint time() const;

    void addAgent(const Agent<Id>& agent);
    /// @brief Add an agent to the simulation
    /// @param agent, Unique pointer to the agent
    void addAgent(std::unique_ptr<Agent<Id>> agent);
    /// @brief Add a pack of agents to the simulation
    /// @param agents, Parameter pack of agents
    template <typename... Tn>
      requires(is_agent_v<Tn> && ...)
    void addAgents(Tn... agents);
    template <typename T1, typename... Tn>
      requires(is_agent_v<T1> && (is_agent_v<Tn> && ...))
    void addAgents(T1 agent, Tn... agents);

    void addRandomAgents(uint nAgents);  // TODO: implement

    /// @brief Add an itinerary
    /// @param itinerary The itinerary
    void addItinerary(const Itinerary<Id>& itinerary);
    /// @brief Add an itinerary
    /// @param itinerary, Unique pointer to the itinerary
    void addItinerary(std::unique_ptr<Itinerary<Id>> itinerary);
    template <typename... Tn>
      requires(is_agent_v<Tn> && ...)
    void addItineraries(Tn... itineraries);
    template <typename T1, typename... Tn>
      requires(is_agent_v<T1> && (is_agent_v<Tn> && ...))
    void addItineraries(T1 itinerary, Tn... itineraries);

    /// @brief Reset the simulation time
    void resetTime();

    template <typename F, typename... Tn>
      requires std::is_invocable_v<F, Tn...>
    void evolve(F f, Tn... args);  // TODO: implement
  };

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  Dynamics<Id, Size>::Dynamics(const Graph<Id, Size>& graph)
      : m_graph{std::make_unique<Graph<Id, Size>>(graph)} {}

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Dynamics<Id, Size>::setItineraries(const std::vector<Itinerary<Id>>& itineraries) {}
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Dynamics<Id, Size>::setAgents(const std::vector<Agent<Id>>& agents) {}

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Dynamics<Id, Size>::setSeed(uint seed) {
    m_generator.seed(seed);
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  const Graph<Id, Size>& Dynamics<Id, Size>::graph() const {
    return *m_graph;
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  const std::vector<std::unique_ptr<Itinerary<Id>>>& Dynamics<Id, Size>::itineraries() const {
    return m_itineraries;
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  const std::vector<std::unique_ptr<Agent<Id>>>& Dynamics<Id, Size>::agents() const {
    return m_agents;
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  TimePoint Dynamics<Id, Size>::time() const {
    return m_time;
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Dynamics<Id, Size>::addAgent(const Agent<Id>& agent) {
    m_agents.insert(std::make_unique<Agent<Id>>(agent));
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Dynamics<Id, Size>::addAgent(std::unique_ptr<Agent<Id>> agent) {
    m_agents.insert(std::move(agent));
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  template <typename... Tn>
    requires(dmf::is_agent_v<Tn> && ...)
  void Dynamics<Id, Size>::addAgents(Tn... agents) {}

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  template <typename T1, typename... Tn>
    requires(dmf::is_agent_v<T1> && (dmf::is_agent_v<Tn> && ...))
  void Dynamics<Id, Size>::addAgents(T1 agent, Tn... agents) {
    addAgent(agent);
    addAgents(agents...);
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Dynamics<Id, Size>::addItinerary(const Itinerary<Id>& itinerary) {}
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Dynamics<Id, Size>::addItinerary(std::unique_ptr<Itinerary<Id>> itinerary) {
    m_itineraries.insert(std::move(itinerary));
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  template <typename... Tn>
    requires(is_agent_v<Tn> && ...)
  void Dynamics<Id, Size>::addItineraries(Tn... itineraries) {}

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  template <typename T1, typename... Tn>
    requires(is_agent_v<T1> && (is_agent_v<Tn> && ...))
  void Dynamics<Id, Size>::addItineraries(T1 itinerary, Tn... itineraries) {
    addItinerary(itinerary);
    addItineraries(itineraries...);
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Dynamics<Id, Size>::resetTime() {
    m_time = 0;
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  template <typename F, typename... Tn>
    requires std::is_invocable_v<F, Tn...>
  void Dynamics<Id, Size>::evolve(F f, Tn... args) {
    f(args...);
  }

};  // namespace dsm

#endif
