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
#include <numeric>
#include <unordered_map>

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
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>
  class Dynamics {
  private:
    std::unordered_map<Id, std::unique_ptr<Itinerary<Id>>> m_itineraries;
    std::unordered_map<Id, std::unique_ptr<Agent<Id, Size, Delay>>> m_agents;
    TimePoint m_time;
    std::unique_ptr<Graph<Id, Size>> m_graph;
    double m_errorProbability;
    double m_minSpeedRateo;
    mutable std::mt19937_64 m_generator{std::random_device{}()};
    std::uniform_real_distribution<double> m_uniformDist{0., 1.};
    std::vector<unsigned int> m_travelTimes;

  public:
    Dynamics() = delete;
    /// @brief Construct a new Dynamics object
    /// @param graph The graph representing the network
    Dynamics(const Graph<Id, Size>& graph);

    /// @brief Set the itineraries
    /// @param itineraries, The itineraries
    void setItineraries(std::span<Itinerary<Id>> itineraries);
    /// @brief Set the seed for the graph's random number generator
    /// @param seed The seed
    void setSeed(unsigned int seed);
    /// @brief Set the minim speed rateo, i.e. the minim speed with respect to the speed limit
    /// @param minSpeedRateo The minim speed rateo
    /// @throw std::invalid_argument, If the minim speed rateo is not between 0 and 1
    void setMinSpeedRateo(double minSpeedRateo);
    /// @brief Set the error probability
    /// @param errorProbability, The error probability
    /// @throw std::invalid_argument, If the error probability is not between 0 and 1
    void setErrorProbability(double errorProbability);
    /// @brief Set the speed of an agent
    /// @tparam Tid The type of the agent's id
    /// @tparam F The type of the function to call
    /// @tparam ...Tn The types of the arguments of the function
    /// @param agentId The index of the agent
    /// @param f The function to call
    /// @param ...args The arguments of the function
    /// @throw std::invalid_argument, If the agent is not found
    template <typename Tid, typename F, typename... Tn>
      requires std::is_invocable_v<F, Tn...>
    void setAgentSpeed(Tid agentId, F f, Tn... args);
    /// @brief Update the paths of the itineraries based on the actual travel times
    void updatePaths();

    /// @brief Get the graph
    /// @return const Graph<Id, Size>&, The graph
    const Graph<Id, Size>& graph() const;
    /// @brief Get the itineraries
    /// @return const std::unordered_map<Id, Itinerary<Id>>&, The itineraries
    const std::unordered_map<Id, std::unique_ptr<Itinerary<Id>>>& itineraries() const;
    /// @brief Get the agents
    /// @return const std::unordered_map<Id, Agent<Id>>&, The agents
    const std::unordered_map<Id, std::unique_ptr<Agent<Id, Size, Delay>>>& agents() const;
    /// @brief Get the time
    /// @return TimePoint, The time
    TimePoint time() const;

    /// @brief Add an agent to the simulation
    /// @param agent The agent
    void addAgent(const Agent<Id, Size, Delay>& agent);
    /// @brief Add an agent to the simulation
    /// @param agent Unique pointer to the agent
    void addAgent(std::unique_ptr<Agent<Id, Size, Delay>> agent);
    /// @brief Add a pack of agents to the simulation
    /// @param agents Parameter pack of agents
    template <typename... Tn>
      requires(is_agent_v<Tn> && ...)
    void addAgents(Tn... agents);
    /// @brief Add a pack of agents to the simulation
    /// @param agent An agent
    /// @param agents Parameter pack of agents
    template <typename T1, typename... Tn>
      requires(is_agent_v<T1> && (is_agent_v<Tn> && ...))
    void addAgents(T1 agent, Tn... agents);
    /// @brief Add a set of agents to the simulation
    /// @param agents Generic container of agents, represented by an std::span
    void addAgents(std::span<Agent<Id, Size, Delay>> agents);

    /// @brief Remove an agent from the simulation
    /// @param agentId the index of the agent to remove
    void removeAgent(Size agentId);
    template <typename T1, typename... Tn>
      requires(std::is_convertible_v<T1, Size> &&
               (std::is_convertible_v<Tn, Size> && ...))
    /// @brief Remove a pack of agents from the simulation
    /// @param id the index of the first agent to remove
    /// @param ids the pack of indexes of the agents to remove
    void removeAgents(T1 id, Tn... ids);

    /// @brief Add an itinerary
    /// @param itinerary, The itinerary
    void addItinerary(const Itinerary<Id>& itinerary);
    /// @brief Add an itinerary
    /// @param itinerary Unique pointer to the itinerary
    void addItinerary(std::unique_ptr<Itinerary<Id>> itinerary);
    template <typename... Tn>
      requires(is_itinerary_v<Tn> && ...)
    void addItineraries(Tn... itineraries);
    /// @brief Add a pack of itineraries
    /// @tparam T1
    /// @tparam ...Tn
    /// @param itinerary
    /// @param ...itineraries
    template <typename T1, typename... Tn>
      requires(is_itinerary_v<T1> && (is_itinerary_v<Tn> && ...))
    void addItineraries(T1 itinerary, Tn... itineraries);
    /// @brief Add a set of itineraries
    /// @param itineraries Generic container of itineraries, represented by an std::span
    void addItineraries(std::span<Itinerary<Id>> itineraries);

    /// @brief Reset the simulation time
    void resetTime();

    /// @brief Evolve the simulation
    /// @tparam F The type of the function to call
    /// @tparam ...Tn The types of the arguments of the function
    /// @param f The function to call
    /// @param ...args The arguments of the function
    template <typename F, typename... Tn>
      requires std::is_invocable_v<F, Tn...>
    void evolve(F f, Tn... args);

    /// @brief Get the mean speed of the agents
    /// @return std::pair<double, double> The mean speed of the agents and the standard deviation
    std::pair<double, double> meanSpeed() const;
    /// @brief Compute the mean speed over nStreets randomly selected streets
    /// @param nStreets The number of streets to select
    /// @return std::pair<double, double> The mean speed of the streets and the standard deviation
    std::pair<double, double> meanSpeed(Size nStreets) const;  // TODO: implement
    /// @brief Get the mean density of the streets
    /// @return std::pair<double, double> The mean density of the streets and the standard deviation
    std::pair<double, double> meanDensity() const;
    /// @brief Compute the mean density over nStreets randomly selected streets
    /// @param nStreets The number of streets to select
    /// @return std::pair<double, double> The mean density of the streets and the standard deviation
    std::pair<double, double> meanDensity(Size nStreets) const;  // TODO: implement
    /// @brief Get the mean flow of the streets
    /// @return std::pair<double, double> The mean flow of the streets and the standard deviation
    std::pair<double, double> meanFlow() const;
    /// @brief Compute the mean flow over nStreets randomly selected streets
    /// @param nStreets The number of streets to select
    /// @return std::pair<double, double> The mean flow of the streets and the standard deviation
    std::pair<double, double> meanFlow(Size nStreets) const;
    /// @brief Get the mean travel time of the agents
    /// @return std::pair<double, double> The mean travel time of the agents and the standard
    std::pair<double, double> meanTravelTime() const;
  };

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>
  Dynamics<Id, Size, Delay>::Dynamics(const Graph<Id, Size>& graph)
      : m_graph{std::make_unique<Graph<Id, Size>>(graph)} {}

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>
  void Dynamics<Id, Size, Delay>::setItineraries(std::span<Itinerary<Id>> itineraries) {
    std::ranges::for_each(itineraries, [this](const auto& itinerary) {
      this->m_itineraries.emplace(itinerary.id(),
                                  std::make_unique<Itinerary<Id>>(itinerary));
    });
  }

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>
  void Dynamics<Id, Size, Delay>::setSeed(unsigned int seed) {
    m_generator.seed(seed);
  }

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>
  void Dynamics<Id, Size, Delay>::setMinSpeedRateo(double minSpeedRateo) {
    if (minSpeedRateo < 0. || minSpeedRateo > 1.) {
      std::string errorMsg{"Error at line " + std::to_string(__LINE__) + " in file " +
                           __FILE__ + ": " +
                           "The minim speed rateo must be between 0 and 1"};
      throw std::invalid_argument(errorMsg);
    }
    m_minSpeedRateo = minSpeedRateo;
  }

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>
  void Dynamics<Id, Size, Delay>::setErrorProbability(double errorProbability) {
    if (errorProbability < 0. || errorProbability > 1.) {
      std::string errorMsg{"Error at line " + std::to_string(__LINE__) + " in file " +
                           __FILE__ + ": " +
                           "The error probability must be between 0 and 1"};
      throw std::invalid_argument(errorMsg);
    }
    m_errorProbability = errorProbability;
  }

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>
             template <typename Tid, typename F, typename... Tn>
               requires std::is_invocable_v<F, Tn...>
  void Dynamics<Id, Size, Delay>::setAgentSpeed(Tid agentId, F f, Tn... args) {
    auto agentIt{std::find_if(m_agents.begin(), m_agents.end(), [agentId](auto agent) {
      return agent->index() == agentId;
    })};
    if (agentIt == m_agents.end()) {
      std::string errorMsg{"Error at line " + std::to_string(__LINE__) + " in file " +
                           __FILE__ + ": " + "Agent " + std::to_string(agentId) +
                           " not found"};
      throw std::invalid_argument(errorMsg);
    }
    auto& agent{*agentIt};
    auto& street{this->m_graph->street(agent->position())};
    double speed{f(args...)};
    agentIt->setSpeed(speed);
  }

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>
  void Dynamics<Id, Size, Delay>::updatePaths() {
    const Size dimension = m_graph->adjMatrix()->getRowDim();
    for (auto& itineraryPair : m_itineraries) {
      SparseMatrix<Id, bool> path{dimension, dimension};
      // cycle over the nodes
      for (Size i{0}; i < dimension; ++i) {
        if (i == itineraryPair.second->destination()) {
          continue;
        }
        auto result{m_graph->shortestPath(i, itineraryPair.second->destination())};
        if (!result.has_value()) {
          continue;
        }
        // save the minimum distance between i and the destination
        auto minDistance{result.value().distance()};
        for (auto const& node : m_graph->adjMatrix()->getRow(i)) {
          // init distance from a neighbor node to the destination to zero
          double distance{0.};

          auto streetResult = m_graph->street(i, node.first);
          if (!streetResult.has_value()) {
            continue;
          }
          auto streetLength{streetResult.value()->length()};
          // TimePoint expectedTravelTime{
          //     streetLength};  // / street->maxSpeed()};  // TODO: change into input velocity
          result = m_graph->shortestPath(node.first, itineraryPair.second->destination());
          if (result.has_value()) {
            // if the shortest path exists, save the distance
            distance = result.value().distance();
          } else if (node.first != itineraryPair.second->destination()) {
            // if the node is the destination, the distance is zero, otherwise the iteration is skipped
            continue;
          }

          // if (!(distance > minDistance + expectedTravelTime)) {
          if (!(distance > minDistance + streetLength)) {
            path.insert(i, node.first, true);
          }
        }
        itineraryPair.second->setPath(path);
      }
    }
  }

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>
  const Graph<Id, Size>& Dynamics<Id, Size, Delay>::graph() const {
    return *m_graph;
  }

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>
  const std::unordered_map<Id, std::unique_ptr<Itinerary<Id>>>&
  Dynamics<Id, Size, Delay>::itineraries() const {
    return m_itineraries;
  }

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>
  const std::unordered_map<Id, std::unique_ptr<Agent<Id, Size, Delay>>>&
  Dynamics<Id, Size, Delay>::agents() const {
    return m_agents;
  }

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>
  TimePoint Dynamics<Id, Size, Delay>::time() const {
    return m_time;
  }

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>
  void Dynamics<Id, Size, Delay>::addAgent(const Agent<Id, Size, Delay>& agent) {
    m_agents.emplace(agent.id(), std::make_unique<Agent<Id, Size, Delay>>(agent));
  }

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>
  void Dynamics<Id, Size, Delay>::addAgent(std::unique_ptr<Agent<Id, Size, Delay>> agent) {
    m_agents.emplace(agent->id(), std::move(agent));
  }

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>
             template <typename... Tn>
               requires(is_agent_v<Tn> && ...)
  void Dynamics<Id, Size, Delay>::addAgents(Tn... agents) {}

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>
             template <typename T1, typename... Tn>
               requires(is_agent_v<T1> && (is_agent_v<Tn> && ...))
  void Dynamics<Id, Size, Delay>::addAgents(T1 agent, Tn... agents) {
    addAgent(agent);
    addAgents(agents...);
  }

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>
  void Dynamics<Id, Size, Delay>::addAgents(std::span<Agent<Id, Size, Delay>> agents) {
    std::ranges::for_each(agents, [this](const auto& agent) -> void {
      this->m_agents.emplace(agent.id(), std::make_unique(agent));
    });
  }

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>
  void Dynamics<Id, Size, Delay>::removeAgent(Size agentId) {
    auto agentIt{std::find_if(m_agents.begin(), m_agents.end(), [agentId](auto agent) {
      return agent->index() == agentId;
    })};
    if (agentIt != m_agents.end()) {
      m_agents.erase(agentIt);
    }
  }

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>
             template <typename T1, typename... Tn>
               requires(std::is_convertible_v<T1, Size> &&
                        (std::is_convertible_v<Tn, Size> && ...))
  void Dynamics<Id, Size, Delay>::removeAgents(T1 id, Tn... ids) {
    removeAgent(id);
    removeAgents(ids...);
  }

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>
  void Dynamics<Id, Size, Delay>::addItinerary(const Itinerary<Id>& itinerary) {
    m_itineraries.emplace(itinerary.id(), std::make_unique<Itinerary<Id>>(itinerary));
  }

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>
  void Dynamics<Id, Size, Delay>::addItinerary(std::unique_ptr<Itinerary<Id>> itinerary) {
    m_itineraries.emplace(itinerary->id(), std::move(itinerary));
  }

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>
             template <typename... Tn>
               requires(is_itinerary_v<Tn> && ...)
  void Dynamics<Id, Size, Delay>::addItineraries(Tn... itineraries) {
    (this->addItinerary(itineraries), ...);
  }

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>
  void Dynamics<Id, Size, Delay>::addItineraries(std::span<Itinerary<Id>> itineraries) {
    std::ranges::for_each(itineraries, [this](const auto& itinerary) -> void {
      this->m_itineraries.emplace(itinerary.id(),
                                  std::make_unique<Itinerary<Id>>(itinerary));
    });
  }

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>
  void Dynamics<Id, Size, Delay>::resetTime() {
    m_time = 0;
  }

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>
             template <typename F, typename... Tn>
               requires std::is_invocable_v<F, Tn...>
  void Dynamics<Id, Size, Delay>::evolve(F f, Tn... args) {
    f(args...);
  }

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>
  std::pair<double, double> Dynamics<Id, Size, Delay>::meanSpeed() const {
    if (m_agents.size() == 0) {
      return std::make_pair(0., 0.);
    }
    double mean{std::accumulate(m_agents.cbegin(),
                                m_agents.cend(),
                                0.,
                                [](double sum, const auto& agent) {
                                  return sum + agent.second->speed();
                                }) /
                m_agents.size()};
    double variance{std::accumulate(m_agents.cbegin(),
                                    m_agents.cend(),
                                    0.,
                                    [mean](double sum, const auto& agent) {
                                      return sum +
                                             std::pow(agent.second->speed() - mean, 2);
                                    }) /
                    (m_agents.size() - 1)};
    return std::make_pair(mean, std::sqrt(variance));
  }
  // template <typename Id, typename Size, typename Delay>
  //   requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
  //            is_numeric_v<Delay>
  // std::pair<double, double> Dynamics<Id, Size, Delay>::meanSpeed(Size nStreets) const {
  //   if (m_graph->streetSet().size() == 0) {
  //     return std::make_pair(0., 0.);
  //   }
  //   std::vector<Id> streetIds;
  //   streetIds.reserve(m_graph->streetSet().size());
  //   for (auto const& street : m_graph->streetSet()) {
  //     streetIds.push_back(street.first);
  //   }
  //   std::sample(streetIds.begin(),
  //               streetIds.end(),
  //               std::back_inserter(streetIds),
  //               nStreets,
  //               m_generator);
  //   // find all agents on the selected streets
  //   std::vector<std::reference_wrapper<const Agent<Id, Size, Delay>>> agents;
  //   for (auto const& agent : m_agents) {
  //     if (std::find(streetIds.begin(), streetIds.end(), agent.second->streetId()) !=
  //         streetIds.end()) {
  //       agents.push_back(std::cref(*agent.second));
  //     }
  //   }
  //   if (agents.size() == 0) {
  //     return std::make_pair(0., 0.);
  //   }
  //   double mean{std::accumulate(agents.cbegin(),
  //                               agents.cend(),
  //                               0.,
  //                               [](double sum, const auto& agent) {
  //                                 return sum + agent.get().speed();
  //                               }) /
  //               agents.size()};
  //   double variance{std::accumulate(agents.cbegin(),
  //                                   agents.cend(),
  //                                   0.,
  //                                   [mean](double sum, const auto& agent) {
  //                                     return sum +
  //                                            std::pow(agent.get().speed() - mean, 2);
  //                                   }) /
  //                   (agents.size() - 1)};
  //   return std::make_pair(mean, std::sqrt(variance));
  // }

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>
  std::pair<double, double> Dynamics<Id, Size, Delay>::meanDensity() const {
    if (m_graph->streetSet().size() == 0) {
      return std::make_pair(0., 0.);
    }
    double mean{std::accumulate(m_graph->streetSet().cbegin(),
                                m_graph->streetSet().cend(),
                                0.,
                                [](double sum, const auto& street) {
                                  return sum + street.second->density();
                                }) /
                m_graph->streetSet().size()};
    double variance{std::accumulate(m_graph->streetSet().cbegin(),
                                    m_graph->streetSet().cend(),
                                    0.,
                                    [mean](double sum, const auto& street) {
                                      return sum +
                                             std::pow(street.second->density() - mean, 2);
                                    }) /
                    (m_graph->streetSet().size() - 1)};
    return std::make_pair(mean, std::sqrt(variance));
  }

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>
  std::pair<double, double> Dynamics<Id, Size, Delay>::meanFlow() const {
    auto meanSpeed{this->meanSpeed()};
    auto meanDensity{this->meanDensity()};

    double mean{meanSpeed.first * meanDensity.first};
    if (mean == 0.) {
      return std::make_pair(0., 0.);
    }
    double variance{(meanSpeed.first * std::pow(meanDensity.second, 2) +
                     std::pow(meanSpeed.second, 2) * meanDensity.first) /
                    mean};
    return std::make_pair(mean, std::sqrt(variance));
  }

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>
  std::pair<double, double> Dynamics<Id, Size, Delay>::meanTravelTime() const {
    if (m_travelTimes.size() == 0) {
      return std::make_pair(0., 0.);
    }
    double mean{std::accumulate(m_travelTimes.cbegin(), m_travelTimes.cend(), 0.) /
                m_travelTimes.size()};
    double variance{std::accumulate(m_travelTimes.cbegin(),
                                    m_travelTimes.cend(),
                                    0.,
                                    [mean](double sum, const auto& travelTime) {
                                      return sum + std::pow(travelTime - mean, 2);
                                    }) /
                    (m_travelTimes.size() - 1)};
    return std::make_pair(mean, std::sqrt(variance));
  }

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             std::unsigned_integral<Delay>
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

    /// @brief Evolve the simulation
    void evolve(bool reinsert_agents = false);
  };

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             std::unsigned_integral<Delay>
  FirstOrderDynamics<Id, Size, Delay>::FirstOrderDynamics(const Graph<Id, Size>& graph)
      : Dynamics<Id, Size, Delay>(graph) {}

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             std::unsigned_integral<Delay>
  void FirstOrderDynamics<Id, Size, Delay>::setAgentSpeed(Size agentId) {
    auto agentIt{std::find_if(m_agents.begin(), m_agents.end(), [agentId](auto agent) {
      return agent->index() == agentId;
    })};
    if (agentIt == m_agents.end()) {
      std::string errorMsg{"Error at line " + std::to_string(__LINE__) + " in file " +
                           __FILE__ + ": " + "Agent " + std::to_string(agentId) +
                           " not found"};
      throw std::invalid_argument(errorMsg);
    }
    auto& agent{*agentIt};
    auto& street{this->m_graph->street(agent->position())};
    double speed{street->maxSpeed() * (1. - this->m_minSpeedRateo * street->density())};
    agentIt->setSpeed(speed);
  }

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             std::unsigned_integral<Delay>
  void FirstOrderDynamics<Id, Size, Delay>::evolve(bool reinsert_agents) {
    for (auto& agent : m_agents) {
      if (!(agent->delay() > 0)) {
        // TODO: check if agent can move and move it if possible
      }
      agent->decrementDelay();
      agent->incrementTravelTime();
      // TODO: implement the rest of the funciton
    }
    ++this->m_time;
  }

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
