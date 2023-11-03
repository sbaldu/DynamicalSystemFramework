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

#include "Agent.hpp"
#include "Itinerary.hpp"
#include "Graph.hpp"
#include "utility/TypeTraits/is_agent.hpp"
#include "utility/TypeTraits/is_itinerary.hpp"

namespace dsm {

  // Alias for shared pointers
  template <typename T>
  using shared = std::shared_ptr<T>;
  using std::make_shared;

  using TimePoint = long long unsigned int;

  /// @brief The Dynamics class represents the dynamics of the network.
  /// @tparam Id, The type of the graph's id. It must be an unsigned integral type.
  /// @tparam Size, The type of the graph's capacity. It must be an unsigned integral type.
  template <typename Id, typename Size>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size>
  class Dynamics {
  private:
    std::vector<Itinerary<Id>> m_itineraries;
    std::vector<shared<Agent<Id>>> m_agents;
    TimePoint m_time;
    shared<Graph<Id, Size>> m_graph;
    double m_temperature;

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

    /// @brief Get the graph
    /// @return const Graph<Id, Size>& The graph
    const Graph<Id, Size>& graph() const;
    /// @brief Get the itineraries
    /// @return const std::vector<Itinerary<Id>>& The itineraries
    const std::vector<shared<Itinerary<Id>>>& itineraries() const;
    /// @brief Get the agents
    /// @return const std::vector<Agent<Id>>& The agents
    const std::vector<shared<Agent<Id>>>& agents() const;
    /// @brief Get the time
    /// @return TimePoint The time
    TimePoint time() const;

    void addAgent(const Agent<Id>& agent);
    void addAgent(shared<Agent<Id>> agent);
    template <typename... Tn>
      requires(is_agent_v<Tn> && ...)
    void addAgents(Tn... agents);
    template <typename T1, typename... Tn>
      requires(is_agent_v<T1> && (is_agent_v<Tn> && ...))
    void addAgents(T1 agent, Tn... agents);

    void addItinerary(const Itinerary<Id>& itinerary);
    void addItinerary(shared<Itinerary<Id>> itinerary);
    template <typename... Tn>
      requires(is_agent_v<Tn> && ...)
    void addItineraries(Tn... itineraries);
    template <typename T1, typename... Tn>
      requires(is_agent_v<T1> && (is_agent_v<Tn> && ...))
    void addItineraries(T1 itinerary, Tn... itineraries);

    void resetTime();

    template <typename F, typename... Tn>
      requires std::is_invocable_v<F, Tn...>
    void evolve(F f, Tn... args);
  };

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  Dynamics<Id, Size>::Dynamics(const Graph<Id, Size>& graph)
      : m_graph{make_shared<Graph<Id, Size>>(graph)} {}

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Dynamics<Id, Size>::setItineraries(const std::vector<Itinerary<Id>>& itineraries) {}
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Dynamics<Id, Size>::setAgents(const std::vector<Agent<Id>>& agents) {}

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  const Graph<Id, Size>& Dynamics<Id, Size>::graph() const {
    return *m_graph;
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  const std::vector<shared<Itinerary<Id>>>& Dynamics<Id, Size>::itineraries() const {
    return m_itineraries;
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  const std::vector<shared<Agent<Id>>>& Dynamics<Id, Size>::agents() const {
    return m_agents;
  }
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  TimePoint Dynamics<Id, Size>::time() const {
    return m_time;
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Dynamics<Id, Size>::addAgent(const Agent<Id>& agent) {}
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Dynamics<Id, Size>::addAgent(shared<Agent<Id>> agent) {}
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  template <typename... Tn>
    requires(is_agent_v<Tn> && ...)
  void Dynamics<Id, Size>::addAgents(Tn... agents) {}
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  template <typename T1, typename... Tn>
    requires(is_agent_v<T1> && (is_agent_v<Tn> && ...))
  void Dynamics<Id, Size>::addAgents(T1 agent, Tn... agents) {
    addAgent(agent);
    addAgents(agents...);
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Dynamics<Id, Size>::addItinerary(const Itinerary<Id>& itinerary) {}
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Dynamics<Id, Size>::addItinerary(shared<Itinerary<Id>> itinerary) {}
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
