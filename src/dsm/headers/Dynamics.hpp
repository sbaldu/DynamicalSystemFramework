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
    std::unordered_map<Id, std::shared_ptr<Itinerary<Id>>> m_itineraries;
    std::unordered_map<Id, std::shared_ptr<Agent<Id, Size, Delay>>> m_agents;
    TimePoint m_time;
    std::shared_ptr<Graph<Id, Size>> m_graph;
    double m_errorProbability;
    double m_minSpeedRateo;
    std::mt19937_64 m_generator{std::random_device{}()};
    std::uniform_real_distribution<double> m_uniformDist{0., 1.};
    std::vector<unsigned int> m_travelTimes;

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
    /// @tparam Tid The type of the agent's id
    /// @tparam F The type of the function to call
    /// @tparam ...Tn The types of the arguments of the function
    /// @param agentId The id of the agent
    /// @param f The function to call
    /// @param ...args The arguments of the function
    /// @throw std::invalid_argument, If the agent is not found
    // template <typename Tid, typename F, typename... Tn>
    //   requires std::is_invocable_v<F, Tn...>
    // void setAgentSpeed(Tid agentId, F f, Tn... args);
    /// @brief Update the paths of the itineraries based on the actual travel times
    void updatePaths();

    /// @brief Get the graph
    /// @return const Graph<Id, Size>&, The graph
    const Graph<Id, Size>& graph() const;
    /// @brief Get the itineraries
    /// @return const std::vector<Itinerary<Id>>&, The itineraries
    const std::unordered_map<Id, std::shared_ptr<Itinerary<Id>>>& itineraries() const;
    /// @brief Get the agents
    /// @return const std::vector<Agent<Id>>&, The agents
    const std::unordered_map<Id, std::shared_ptr<Agent<Id, Size, Delay>>>& agents() const;
    /// @brief Get the time
    /// @return TimePoint, The time
    TimePoint time() const;

    /// @brief Add an agent to the simulation
    /// @param agent, The agent
    void addAgent(const Agent<Id, Size, Delay>& agent);
    /// @brief Add an agent to the simulation
    /// @param agent, Unique pointer to the agent
    void addAgent(std::shared_ptr<Agent<Id, Size, Delay>> agent);
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
    void addItinerary(std::shared_ptr<Itinerary<Id>> itinerary);
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
      : m_graph{std::make_shared<Graph<Id, Size>>(graph)} {}

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Dynamics<Id, Size, Delay>::setItineraries(std::span<Itinerary<Id>> itineraries) {
    std::ranges::for_each(itineraries, [this](const auto& itinerary) {
      this->m_itineraries.insert(std::make_shared<Itinerary<Id>>(itinerary));
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

  // template <typename Id, typename Size, typename Delay>
  //   requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> && is_numeric_v<Delay>)
  // template <typename Tid, typename F, typename... Tn>
  //   requires std::is_invocable_v<F, Tn...>
  // void Dynamics<Id, Size, Delay>::setAgentSpeed(Tid agentId, F f, Tn... args) {
  //   auto agentIt{std::find_if(
  //       m_agents.begin(), m_agents.end(), [agentId](auto agent) { return agent->id() == agentId; })};
  //   if (agentIt == m_agents.end()) {
  //     std::string errorMsg{"Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
  //                          "Agent " + std::to_string(agentId) + " not found"};
  //     throw std::invalid_argument(errorMsg);
  //   }
  //   auto& agent{*agentIt};
  //   auto& street{this->m_graph->street(agent->position())};
  //   double speed{f(args...)};
  //   agentIt->setSpeed(speed);
  // }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>)
  void Dynamics<Id, Size, Delay>::updatePaths() {
    const Size dimension = m_graph->adjMatrix()->getRowDim();
    for (auto& itineraryPair : m_itineraries) {
      auto& itinerary{itineraryPair.second};
      SparseMatrix<Id, bool> path{dimension, dimension};
      // cycle over the nodes
      for (Size i{0}; i < dimension; ++i) {
        if (i == itinerary->destination()) {
          continue;
        }
        auto result{m_graph->shortestPath(i, itinerary->destination())};
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
          result = m_graph->shortestPath(node.first, itinerary->destination());
          if (result.has_value()) {
            // if the shortest path exists, save the distance
            distance = result.value().distance();
          } else if (node.first != itinerary->destination()) {
            // if the node is the destination, the distance is zero, otherwise the iteration is skipped
            continue;
          }

          // if (!(distance > minDistance + expectedTravelTime)) {
          if (!(distance > minDistance + streetLength)) {
            path.insert(i, node.first, true);
          }
        }
        itinerary->setPath(path);
      }
    }
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  const Graph<Id, Size>& Dynamics<Id, Size, Delay>::graph() const {
    return *m_graph;
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  const std::unordered_map<Id, std::shared_ptr<Itinerary<Id>>>& Dynamics<Id, Size, Delay>::itineraries()
      const {
    return m_itineraries;
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  const std::unordered_map<Id, std::shared_ptr<Agent<Id, Size, Delay>>>& Dynamics<Id, Size, Delay>::agents()
      const {
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
    m_agents.emplace(agent.id(), std::make_shared<Agent<Id, Size, Delay>>(agent));
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Dynamics<Id, Size, Delay>::addAgent(std::shared_ptr<Agent<Id, Size, Delay>> agent) {
    m_agents.emplace(agent->id(), agent);
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
        agents, [this](const auto& agent) -> void { this->m_agents.push_back(std::make_shared(agent)); });
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Dynamics<Id, Size, Delay>::removeAgent(Size agentId) {
    try {
      this->m_agents.erase(agentId);
    }
    catch (std::out_of_range& e) {
      std::string errorMsg{"Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
                           "Agent " + std::to_string(agentId) + " not found"};
      throw std::invalid_argument(errorMsg);
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
    m_itineraries.emplace(itinerary.id(), std::make_shared<Itinerary<Id>>(itinerary));
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Dynamics<Id, Size, Delay>::addItinerary(std::shared_ptr<Itinerary<Id>> itinerary) {
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
      this->m_itineraries.push_back(std::make_shared<Itinerary<Id>>(itinerary));
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
    Size nAgents{static_cast<Size>(std::ranges::count_if(m_agents, [](const auto& agent) { return agent.second->streetId().has_value(); }))};
    if (m_agents.size() == 0 || nAgents == 0) {
      return 0.;
    }
    double meanSpeed{std::accumulate(m_agents.cbegin(),
                           m_agents.cend(),
                           0.,
                           [](double sum, const auto& agent) { return sum + agent.second->speed(); })};
    return static_cast<double>(meanSpeed / nAgents);
  }
  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>)
  double Dynamics<Id, Size, Delay>::meanDensity() const {
    if (m_graph->streetSet().size() == 0) {
      return 0.;
    }
    return std::accumulate(m_graph->streetSet().cbegin(),
                           m_graph->streetSet().cend(),
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
    if (m_travelTimes.size() == 0) {
      return 0.;
    }
    return std::accumulate(m_travelTimes.cbegin(), m_travelTimes.cend(), 0.) / m_travelTimes.size();
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
    /// @brief Add a set of agents to the simulation
    /// @param nAgents The number of agents to add
    /// @param uniformly If true, the agents are added uniformly on the streets
    /// @throw std::runtime_error If there are no itineraries
    void addRandomAgents(Size nAgents, bool uniformly = false);

    /// @brief Evolve the simulation
    void evolve(bool reinsert_agents = false);
  };

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>)
  FirstOrderDynamics<Id, Size, Delay>::FirstOrderDynamics(const Graph<Id, Size>& graph)
      : Dynamics<Id, Size, Delay>(graph) {}

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void FirstOrderDynamics<Id, Size, Delay>::setAgentSpeed(Size agentId) {
    auto agent = this->m_agents[agentId];
    auto street{this->m_graph->streetSet()[agent->streetId().value()]};
    double speed{street->maxSpeed() * (1. - this->m_minSpeedRateo * street->density())};
    agent->setSpeed(speed);
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>)
  void FirstOrderDynamics<Id, Size, Delay>::addRandomAgents(Size nAgents, bool uniformly) {
    if (this->m_itineraries.empty()) {
      std::string errorMsg{"Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
                           "It is not possible to add random agents without itineraries"};
      throw std::runtime_error(errorMsg);
    }
    std::uniform_int_distribution<Size> itineraryDist{0, static_cast<Size>(this->m_itineraries.size() - 1)};
    std::uniform_int_distribution<Size> streetDist{0, static_cast<Size>(this->m_graph->streetSet().size() - 1)};
    for (Size i{0}; i < nAgents; ++i) {
      Size itineraryId{itineraryDist(this->m_generator)};
      Size agentId{static_cast<Size>(this->m_agents.size())};
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
        this->addAgent(Agent<Id, Size, Delay>{agentId, itineraryId, streetId});
        auto agent{this->m_agents[agentId]};
        this->setAgentSpeed(agentId);
        agent->incrementDelay(street->length() / agent->speed());
        street->enqueue(agentId);
      } else {
        this->addAgent(Agent<Id, Size, Delay>{agentId, itineraryId});
      }
      std::cout << "Added agent " << agentId << " with itinerary " << itineraryId << " StreetId: " << this->m_agents[agentId]->streetId().value_or(-1) << std::endl;
    }
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>)
  void FirstOrderDynamics<Id, Size, Delay>::evolve(bool reinsert_agents) {
    // move the first agent of each street
    for (auto& streetPair : this->m_graph->streetSet()) {
      auto street{streetPair.second};
      if (street->queue().empty()) {
        continue;
      }
      auto agent = this->m_agents[street->queue().front()];
      if (agent->delay() > 0) {
        continue;
      }
      agent->setSpeed(0.);
      auto destinationNode{this->m_graph->nodeSet()[street->nodePair().second]};
      if (destinationNode->isFull()) {
        continue;
      }
      street->dequeue();
      destinationNode->enqueue(agent->id());
    }
    // move the agents from each node
    for (auto& nodePair : this->m_graph->nodeSet()) {
      auto node{nodePair.second};
      while (!node->queue().empty()) {
        Id agentId{node->queue().front()};
        if (node->id() == this->m_itineraries[this->m_agents[agentId]->itineraryId()]->destination()) {
          this->m_travelTimes.push_back(this->m_agents[agentId]->time());
          node->dequeue();
          if (reinsert_agents) {
            auto newAgent = Agent<Id, Size, Delay>(this->m_agents[agentId]->id(), this->m_agents[agentId]->itineraryId());
            this->removeAgent(agentId);
            this->addAgent(newAgent);
          } else {
            this->removeAgent(agentId);
          }
          continue;
        }
        auto agent = this->m_agents[agentId];
        auto possibleMoves{this->m_itineraries[agent->itineraryId()]->path().getRow(node->id())};
        if (this->m_uniformDist(this->m_generator) < this->m_errorProbability) {
          possibleMoves = this->m_graph->adjMatrix()->getRow(node->id());
        }
        // print possible moves
        // std::cout << "Possible moves from " << node->id() << ": ";
        // for (auto const& move : possibleMoves) {
        //   std::cout << move.first << " ";
        // }
        // std::cout << "\n";
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
          agent->setStreetId(nextStreet->id());
          this->setAgentSpeed(agent->id());
          agent->incrementDelay(nextStreet->length() / agent->speed());
          nextStreet->enqueue(agent->id());
          node->dequeue();
        } else {
          break;
        }
      }
    }
    // cycle over agents and put them in the src node
    for (auto& agentPair : this->m_agents) {
      auto agent{agentPair.second};
      if (agent->streetId().has_value()) {
        continue;
      }
      auto srcNode{this->m_graph->nodeSet()[this->m_itineraries[agent->itineraryId()]->source()]};
      if (srcNode->isFull()) {
        continue;
      }
      srcNode->enqueue(agent->id());
    }
    // decrement all agent delays
    std::ranges::for_each(this->m_agents, [](auto& agent) {
      if (agent.second->delay() > 0) {
        agent.second->decrementDelay();
      };
      agent.second->incrementTime();
    });
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
