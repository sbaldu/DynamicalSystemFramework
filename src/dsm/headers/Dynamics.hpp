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
#include <iterator>

#include "Agent.hpp"
#include "Itinerary.hpp"
#include "Graph.hpp"
#include "SparseMatrix.hpp"
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
  protected:
    std::unordered_map<Id, std::unique_ptr<Itinerary<Id>>> m_itineraries;
    std::unordered_map<Id, std::unique_ptr<Agent<Id, Size, Delay>>> m_agents;
    TimePoint m_time;
    std::unique_ptr<Graph<Id, Size>> m_graph;
    double m_errorProbability;
    double m_minSpeedRateo;
    std::mt19937_64 m_generator{std::random_device{}()};
    std::uniform_real_distribution<double> m_uniformDist{0., 1.};
    std::vector<unsigned int> m_travelTimes;

    /// @brief Evolve the streets
    /// @details If possible, removes the first agent of each street queue, putting it in the destination node.
    void m_evolveStreets();
    void m_evolveNodes(bool reinsert_agents);
    /// @brief Evolve the agents.
    /// @details Puts all new agents on a street, if possible, decrements all delays
    /// and increments all travel times.
    void m_evolveAgents();

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
    /// @brief Set the error probability
    /// @param errorProbability, The error probability
    /// @throw std::invalid_argument, If the error probability is not between 0 and 1
    void setErrorProbability(double errorProbability);
    /// @brief Set the speed of an agent
    /// @details This is a pure-virtual function, it must be implemented in the derived classes
    /// @param agentId The id of the agent 
    virtual void setAgentSpeed(Size agentId) = 0;

    /// @brief Update the paths of the itineraries based on the actual travel times
    void updatePaths();
    /// @brief Evolve the simulation
    /// @details Evolve the simulation by moving the agents and updating the travel times.
    /// In particular:
    /// - Move the first agent of each street queue, if possible, putting it in the next node
    /// - Move the agents from each node, if possible, putting them in the next street and giving them a speed.
    /// If the error probability is not zero, the agents can move to a random street.
    /// If the agent is in the destination node, it is removed from the simulation (and then reinserted if reinsert_agents is true)
    /// - Cycle over agents and update their times
    /// @param reinsert_agents If true, the agents are reinserted in the simulation after they reach their destination
    void evolve(bool reinsert_agents = false);

    /// @brief Get the graph
    /// @return const Graph<Id, Size>&, The graph
    const Graph<Id, Size>& graph() const;
    /// @brief Get the itineraries
    /// @return const std::vector<Itinerary<Id>>&, The itineraries
    const std::unordered_map<Id, std::unique_ptr<Itinerary<Id>>>& itineraries() const;
    /// @brief Get the agents
    /// @return const std::vector<Agent<Id>>&, The agents
    const std::unordered_map<Id, std::unique_ptr<Agent<Id, Size, Delay>>>& agents() const;
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
    /// @brief Add a set of agents to the simulation
    /// @param nAgents The number of agents to add
    /// @param uniformly If true, the agents are added uniformly on the streets
    /// @throw std::runtime_error If there are no itineraries
    void addRandomAgents(Size nAgents, bool uniformly = false);

    /// @brief Remove an agent from the simulation
    /// @param agentId the id of the agent to remove
    void removeAgent(Size agentId);
    template <typename T1, typename... Tn>
      requires(std::is_convertible_v<T1, Size> && (std::is_convertible_v<Tn, Size> && ...))
    /// @brief Remove a pack of agents from the simulation
    /// @param id, the id of the first agent to remove
    /// @param ids, the pack of ides of the agents to remove
    void removeAgents(T1 id, Tn... ids);

    /// @brief Add an itinerary
    /// @param itinerary, The itinerary
    void addItinerary(const Itinerary<Id>& itinerary);
    /// @brief Add an itinerary
    /// @param itinerary, Unique pointer to the itinerary
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
    /// @param itineraries, Generic container of itineraries, represented by an std::span
    void addItineraries(std::span<Itinerary<Id>> itineraries);

    /// @brief Reset the simulation time
    void resetTime();
    /// @brief Move an agent
    /// @param agentId The id of the agent to move
    /// @return true If the agent has been moved, false otherwise
    bool moveAgent(Size agentId);

    /// @brief Evolve the simulation
    /// @tparam F The type of the function to call
    /// @tparam ...Tn The types of the arguments of the function
    /// @param f The function to call
    /// @param ...args The arguments of the function
    template <typename F, typename... Tn>
      requires std::is_invocable_v<F, Tn...>
    void evolve(F f, Tn... args);

    /// @brief Get the mean speed of the agents
    /// @return double, The mean speed of the agents
    double meanSpeed() const;
    /// @brief Get the mean density of the streets
    /// @return double, The mean density of the streets
    double meanDensity() const;
    /// @brief Get the mean flow of the streets
    /// @return double, The mean flow of the streets
    double meanFlow() const;
    /// @brief Get the mean travel time of the agents
    /// @return double, The mean travel time of the agents
    double meanTravelTime() const;
  };

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  Dynamics<Id, Size, Delay>::Dynamics(const Graph<Id, Size>& graph)
      : m_time{0}, m_graph{std::make_unique<Graph<Id, Size>>(graph)}, m_errorProbability{0.}, m_minSpeedRateo{0.} {}

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> && is_numeric_v<Delay>)
  void Dynamics<Id, Size, Delay>::m_evolveStreets() {
    for (auto& streetPair : this->m_graph->streetSet()) {
      auto street{streetPair.second};
      if (street->queue().empty()) {
        continue;
      }
      Id agentId{street->queue().front()};
      if (this->m_agents[agentId]->delay() > 0) {
        continue;
      }
      this->m_agents[agentId]->setSpeed(0.);
      auto destinationNode{this->m_graph->nodeSet()[street->nodePair().second]};
      if (destinationNode->isFull()) {
        continue;
      }
      destinationNode->enqueue(street->dequeue().value());
    }
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> && is_numeric_v<Delay>)
  void Dynamics<Id, Size, Delay>::m_evolveNodes(bool reinsert_agents) {
    for (auto& nodePair : this->m_graph->nodeSet()) {
      auto node{nodePair.second};
      while (!node->queue().empty()) {
        Id agentId{node->queue().front()};
        if (node->id() == this->m_itineraries[this->m_agents[agentId]->itineraryId()]->destination()) {
          agentId = node->dequeue().value();
          this->m_travelTimes.push_back(this->m_agents[agentId]->time());
          if (reinsert_agents) {
            Agent<Id, Size, Delay> newAgent{this->m_agents[agentId]->id(), this->m_agents[agentId]->itineraryId()};
            this->removeAgent(agentId);
            this->addAgent(newAgent);
          } else {
            this->removeAgent(agentId);
          }
          continue;
        }
        auto possibleMoves{this->m_itineraries[this->m_agents[agentId]->itineraryId()]->path().getRow(node->id())};
        if (this->m_uniformDist(this->m_generator) < this->m_errorProbability) {
          possibleMoves = this->m_graph->adjMatrix()->getRow(node->id());
        }
        if (static_cast<Size>(possibleMoves.size()) == 0) {
          continue;
        }
        std::uniform_int_distribution<Size> moveDist{0, static_cast<Size>(possibleMoves.size() - 1)};
        const auto p{moveDist(this->m_generator)};
        auto iterator = possibleMoves.begin();
        std::advance(iterator, p);
        const auto& streetResult{this->m_graph->street(node->id(), iterator->first)};
        if (!streetResult.has_value()) {
          continue;
        }
        auto nextStreet{streetResult.value()};

        if (nextStreet->density() < 1) {
          agentId = node->dequeue().value();
          this->m_agents[agentId]->setStreetId(nextStreet->id());
          this->setAgentSpeed(this->m_agents[agentId]->id());
          this->m_agents[agentId]->incrementDelay(nextStreet->length() / this->m_agents[agentId]->speed());
          nextStreet->enqueue(this->m_agents[agentId]->id());
        } else {
          break;
        }
      }
    }
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> && is_numeric_v<Delay>)
  void Dynamics<Id, Size, Delay>::m_evolveAgents() {
    for (auto& agentPair : this->m_agents) {
      if (agentPair.second->streetId().has_value()) {
        continue;
      }
      auto srcNode{this->m_graph->nodeSet()[this->m_itineraries[agentPair.second->itineraryId()]->source()]};
      if (srcNode->isFull()) {
        continue;
      }
      srcNode->enqueue(agentPair.second->id());
    }
    // decrement all agent delays
    std::ranges::for_each(this->m_agents, [](auto& agent) {
      if (agent.second->delay() > 0) {
        agent.second->decrementDelay();
      };
      agent.second->incrementTime();
    });
  }

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
  void Dynamics<Id, Size, Delay>::setErrorProbability(double errorProbability) {
    if (errorProbability < 0. || errorProbability > 1.) {
      std::string errorMsg{"Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
                           "The error probability must be between 0 and 1"};
      throw std::invalid_argument(errorMsg);
    }
    m_errorProbability = errorProbability;
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>)
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
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Dynamics<Id, Size, Delay>::evolve(bool reinsert_agents) {
    // move the first agent of each street queue, if possible, putting it in the next node
    this->m_evolveStreets();
    // move all the agents from each node, if possible
    this->m_evolveNodes(reinsert_agents);
    // cycle over agents and update their times
    this->m_evolveAgents();
    // increment time simulation
    ++this->m_time;
  }


  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  const Graph<Id, Size>& Dynamics<Id, Size, Delay>::graph() const {
    return *m_graph;
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  const std::unordered_map<Id, std::unique_ptr<Itinerary<Id>>>& Dynamics<Id, Size, Delay>::itineraries()
      const {
    return m_itineraries;
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  const std::unordered_map<Id, std::unique_ptr<Agent<Id, Size, Delay>>>& Dynamics<Id, Size, Delay>::agents()
      const {
    return this->m_agents;
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  TimePoint Dynamics<Id, Size, Delay>::time() const {
    return m_time;
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Dynamics<Id, Size, Delay>::addAgent(const Agent<Id, Size, Delay>& agent) {
    this->m_agents.insert(std::make_pair(agent.id(), std::make_unique<Agent<Id, Size, Delay>>(agent)));
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Dynamics<Id, Size, Delay>::addAgent(std::unique_ptr<Agent<Id, Size, Delay>> agent) {
    this->m_agents.insert(std::make_pair(agent->id(), std::move(agent)));
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
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>)
  void Dynamics<Id, Size, Delay>::addRandomAgents(Size nAgents, bool uniformly) {
    if (this->m_itineraries.empty()) {
      std::string errorMsg{"Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
                           "It is not possible to add random agents without itineraries"};
      throw std::runtime_error(errorMsg);
    }
    std::uniform_int_distribution<Size> itineraryDist{0, static_cast<Size>(this->m_itineraries.size() - 1)};
    std::uniform_int_distribution<Size> streetDist{0, static_cast<Size>(this->m_graph->streetSet().size() - 1)};
    for (Size i{0}; i < nAgents; ++i) {
      Size itineraryId{itineraryDist(this->m_generator)};
      // find the max of m_agents keys

      Size agentId{0};
      if (!this->m_agents.empty()) {
        agentId = std::max_element(this->m_agents.cbegin(), this->m_agents.cend(), [](const auto& a, const auto& b) {
          return a.first < b.first;
        })->first + 1;
      }
      if(uniformly) {
        Size streetId{0};
        do {
          // I dunno why this works and the following doesn't
          auto streetSet = this->m_graph->streetSet();
          auto streetIt = streetSet.begin();
          // auto streetIt = this->m_graph->streetSet().begin();
          Size step = streetDist(this->m_generator);
          std::advance(streetIt, step);
          streetId = streetIt->first;
        } while (this->m_graph->streetSet()[streetId]->density() == 1);
        auto street{this->m_graph->streetSet()[streetId]};
        Agent<Id, Size, Delay> agent{agentId, itineraryId, streetId};
        this->addAgent(agent);
        this->setAgentSpeed(agentId);
        this->m_agents[agentId]->incrementDelay(street->length() / this->m_agents[agentId]->speed());
        street->enqueue(agentId);
      } else {
        this->addAgent(Agent<Id, Size, Delay>{agentId, itineraryId});
      }
      ++agentId;
    }
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Dynamics<Id, Size, Delay>::removeAgent(Size agentId) {
    auto agentIt{m_agents.find(agentId)};
    if (agentIt == m_agents.end()) {
      std::string errorMsg{"Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
                           "Agent " + std::to_string(agentId) + " not found"};
      throw std::invalid_argument(errorMsg);
    }
    m_agents.erase(agentIt);
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
    m_itineraries.emplace(itinerary.id(), std::make_unique<Itinerary<Id>>(itinerary));
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Dynamics<Id, Size, Delay>::addItinerary(std::unique_ptr<Itinerary<Id>> itinerary) {
    m_itineraries.emplace(itinerary.id(), std::move(itinerary));
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
  double Dynamics<Id, Size, Delay>::meanSpeed() const {
    // count agents which have street not nullopt
    Size nAgents{static_cast<Size>(std::ranges::count_if(this->m_agents, [](const auto& agent) { return agent.second->streetId().has_value(); }))};
    if (this->m_agents.size() == 0 || nAgents == 0) {
      return 0.;
    }
    double meanSpeed{std::accumulate(this->m_agents.cbegin(),
                           this->m_agents.cend(),
                           0.,
                           [](double sum, const auto& agent) { return sum + agent.second->speed(); })};
    return static_cast<double>(meanSpeed / nAgents);
  }
  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>)
  double Dynamics<Id, Size, Delay>::meanDensity() const {
    if (this->m_graph->streetSet().size() == 0) {
      return 0.;
    }
    return std::accumulate(this->m_graph->streetSet().cbegin(),
                           this->m_graph->streetSet().cend(),
                           0.,
                           [](double sum, const auto& street) { return sum + street.second->density(); }) /
           m_graph->streetSet().size();
  }
  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>)
  double Dynamics<Id, Size, Delay>::meanFlow() const {
    return this->meanDensity() * this->meanSpeed();
  }
  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>)
  double Dynamics<Id, Size, Delay>::meanTravelTime() const {
    if (this->m_travelTimes.size() == 0) {
      return 0.;
    }
    return std::accumulate(this->m_travelTimes.cbegin(), this->m_travelTimes.cend(), 0.) / this->m_travelTimes.size();
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>)
  class FirstOrderDynamics : public Dynamics<Id, Size, Delay> {
  public:
    FirstOrderDynamics() = delete;
    /// @brief Construct a new First Order Dynamics object
    /// @param graph, The graph representing the network
    FirstOrderDynamics(const Graph<Id, Size>& graph);
    /// @brief Set the speed of an agent
    /// @param agentId The id of the agent
    /// @throw std::invalid_argument, If the agent is not found
    void setAgentSpeed(Size agentId);
  };

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>)
  FirstOrderDynamics<Id, Size, Delay>::FirstOrderDynamics(const Graph<Id, Size>& graph)
      : Dynamics<Id, Size, Delay>(graph) {}

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void FirstOrderDynamics<Id, Size, Delay>::setAgentSpeed(Size agentId) {
    auto street{this->m_graph->streetSet()[this->m_agents[agentId]->streetId().value()]};
    double speed{street->maxSpeed() * (1. - this->m_minSpeedRateo * street->density())};
    this->m_agents[agentId]->setSpeed(speed);
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
