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
#include <numeric>
#include <unordered_map>
#include <cmath>

#include "Agent.hpp"
#include "Itinerary.hpp"
#include "Graph.hpp"
#include "SparseMatrix.hpp"
#include "../utility/TypeTraits/is_agent.hpp"
#include "../utility/TypeTraits/is_itinerary.hpp"
#include "../utility/Logger.hpp"

namespace dsm {

  using TimePoint = long long unsigned int;

  /// @brief The Measurement struct represents the mean of a quantity and its standard deviation
  /// @tparam T The type of the mean and the standard deviation
  /// @param mean The mean
  /// @param error The standard deviation of the sample
  template <typename T>
  struct Measurement {
    T mean;
    T error;

    Measurement(T mean, T error) : mean{mean}, error{error} {}
    Measurement(const std::vector<T>& data) {
      if (data.size() == 0) {
        mean = 0.;
        error = 0.;
      } else {
        mean = std::accumulate(data.cbegin(), data.cend(), 0.) / data.size();
        if (data.size() < 2) {
          error = 0.;
        } else {
          const double cvariance{std::accumulate(data.cbegin(),
                                                 data.cend(),
                                                 0.,
                                                 [this](double sum, const auto& value) {
                                                   return sum + std::pow(value - mean, 2);
                                                 }) /
                                 (data.size() - 1)};
          error = std::sqrt(cvariance);
        }
      }
    }
  };

  /// @brief The Dynamics class represents the dynamics of the network.
  /// @tparam Id, The type of the graph's id. It must be an unsigned integral type.
  /// @tparam Size, The type of the graph's capacity. It must be an unsigned integral type.
  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  class Dynamics {
  protected:
    std::unordered_map<Id, std::unique_ptr<Itinerary<Id>>> m_itineraries;
    std::map<Id, std::unique_ptr<Agent<Id, Size, Delay>>> m_agents;
    TimePoint m_time;
    Graph<Id, Size> m_graph;
    double m_errorProbability;
    double m_minSpeedRateo;
    mutable std::mt19937_64 m_generator{std::random_device{}()};
    std::uniform_real_distribution<double> m_uniformDist{0., 1.};
    std::vector<unsigned int> m_travelTimes;

    /// @brief Evolve the streets
    /// @details If possible, removes the first agent of each street queue, putting it in the destination node.
    virtual void m_evolveStreets();
    /// @brief Evolve the nodes
    /// @details If possible, removes all agents from each node, putting them in the next street.
    /// If the error probability is not zero, the agents can move to a random street.
    /// If the agent is in the destination node, it is removed from the simulation (and then reinserted if reinsert_agents is true)
    /// @param reinsert_agents If true, the agents are reinserted in the simulation after they reach their destination
    virtual void m_evolveNodes(bool reinsert_agents);
    /// @brief Evolve the agents.
    /// @details Puts all new agents on a street, if possible, decrements all delays
    /// and increments all travel times.
    virtual void m_evolveAgents();

  public:
    Dynamics() = delete;
    /// @brief Construct a new Dynamics object
    /// @param graph The graph representing the network
    Dynamics(Graph<Id, Size>& graph);

    /// @brief Set the itineraries
    /// @param itineraries The itineraries
    void setItineraries(std::span<Itinerary<Id>> itineraries);
    /// @brief Set the seed for the graph's random number generator
    /// @param seed The seed
    void setSeed(unsigned int seed) { m_generator.seed(seed); };
    /// @brief Set the minim speed rateo, i.e. the minim speed with respect to the speed limit
    /// @param minSpeedRateo The minim speed rateo
    /// @throw std::invalid_argument If the minim speed rateo is not between 0 and 1
    void setMinSpeedRateo(double minSpeedRateo);
    /// @brief Set the error probability
    /// @param errorProbability The error probability
    /// @throw std::invalid_argument If the error probability is not between 0 and 1
    void setErrorProbability(double errorProbability);
    /// @brief Set the speed of an agent
    /// @details This is a pure-virtual function, it must be implemented in the derived classes
    /// @param agentId The id of the agent
    virtual void setAgentSpeed(Size agentId) = 0;

    /// @brief Update the paths of the itineraries based on the actual travel times
    virtual void updatePaths();
    /// @brief Evolve the simulation
    /// @details Evolve the simulation by moving the agents and updating the travel times.
    /// In particular:
    /// - Move the first agent of each street queue, if possible, putting it in the next node
    /// - Move the agents from each node, if possible, putting them in the next street and giving them a speed.
    /// If the error probability is not zero, the agents can move to a random street.
    /// If the agent is in the destination node, it is removed from the simulation (and then reinserted if reinsert_agents is true)
    /// - Cycle over agents and update their times
    /// @param reinsert_agents If true, the agents are reinserted in the simulation after they reach their destination
    virtual void evolve(bool reinsert_agents = false);

    /// @brief Get the graph
    /// @return const Graph<Id, Size>&, The graph
    const Graph<Id, Size>& graph() const { return m_graph; };
    /// @brief Get the itineraries
    /// @return const std::unordered_map<Id, Itinerary<Id>>&, The itineraries
    const std::unordered_map<Id, std::unique_ptr<Itinerary<Id>>>& itineraries() const {
      return m_itineraries;
    }
    /// @brief Get the agents
    /// @return const std::unordered_map<Id, Agent<Id>>&, The agents
    const std::map<Id, std::unique_ptr<Agent<Id, Size, Delay>>>& agents() const {
      return m_agents;
    }
    /// @brief Get the time
    /// @return TimePoint The time
    TimePoint time() const { return m_time; }

    /// @brief Add an agent to the simulation
    /// @param agent The agent
    void addAgent(const Agent<Id, Size, Delay>& agent);
    /// @brief Add an agent to the simulation
    /// @param agent std::unique_ptr to the agent
    void addAgent(std::unique_ptr<Agent<Id, Size, Delay>> agent);
    /// @brief Add a pack of agents to the simulation
    /// @param itineraryId The index of the itinerary
    /// @param nAgents The number of agents to add
    /// @throw std::invalid_argument If the itinerary is not found
    /// @details adds nAgents agents with the same itinerary of id itineraryId
    void addAgents(Id itineraryId,
                   Size nAgents = 1,
                   std::optional<Id> srcNodeId = std::nullopt);
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
    /// @brief Add a set of agents to the simulation
    /// @param nAgents The number of agents to add
    /// @param uniformly If true, the agents are added uniformly on the streets
    /// @throw std::runtime_error If there are no itineraries
    virtual void addAgentsUniformly(Size nAgents,
                                    std::optional<Id> itineraryId = std::nullopt);

    /// @brief Remove an agent from the simulation
    /// @param agentId the id of the agent to remove
    void removeAgent(Size agentId);
    template <typename T1, typename... Tn>
      requires(std::is_convertible_v<T1, Size> &&
               (std::is_convertible_v<Tn, Size> && ...))
    /// @brief Remove a pack of agents from the simulation
    /// @param id the id of the first agent to remove
    /// @param ids the pack of ides of the agents to remove
    void removeAgents(T1 id, Tn... ids);

    /// @brief Add an itinerary
    /// @param itinerary The itinerary
    void addItinerary(const Itinerary<Id>& itinerary);
    /// @brief Add an itinerary
    /// @param itinerary std::unique_ptr to the itinerary
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
    /// @return Measurement<double> The mean speed of the agents and the standard deviation
    Measurement<double> meanSpeed() const;
    // TODO: implement the following functions
    // We can implement the base version of these functions by cycling over agents... I won't do it for now.
    // Grufoony - 19/02/2024
    virtual std::optional<double> streetMeanSpeed(Id) const = 0;
    virtual Measurement<double> streetMeanSpeed() const = 0;
    virtual Measurement<double> streetMeanSpeed(double, bool) const = 0;
    /// @brief Get the mean density of the streets
    /// @return Measurement<double> The mean density of the streets and the standard deviation
    Measurement<double> streetMeanDensity() const;
    /// @brief Get the mean flow of the streets
    /// @return Measurement<double> The mean flow of the streets and the standard deviation
    Measurement<double> streetMeanFlow() const;
    /// @brief Get the mean flow of the streets
    /// @param threshold The density threshold to consider
    /// @param above If true, the function returns the mean flow of the streets with a density above the threshold, otherwise below
    /// @return Measurement<double> The mean flow of the streets and the standard deviation
    Measurement<double> streetMeanFlow(double threshold, bool above) const;
    /// @brief Get the mean travel time of the agents
    /// @param clearData If true, the travel times are cleared after the computation
    /// @return Measurement<double> The mean travel time of the agents and the standard
    Measurement<double> meanTravelTime(bool clearData = false);
  };

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  Dynamics<Id, Size, Delay>::Dynamics(Graph<Id, Size>& graph)
      : m_time{0},
        m_graph{std::move(graph)},
        m_errorProbability{0.},
        m_minSpeedRateo{0.} {}

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  void Dynamics<Id, Size, Delay>::m_evolveStreets() {
    for (auto& [streetId, street] : this->m_graph.streetSet()) {
      if (street->queue().empty()) {
        continue;
      }
      Size agentId{street->queue().front()};
      if (this->m_agents[agentId]->delay() > 0) {
        continue;
      }
      this->m_agents[agentId]->setSpeed(0.);
      auto& destinationNode{this->m_graph.nodeSet()[street->nodePair().second]};
      if (destinationNode->isFull()) {
        continue;
      }
      if (destinationNode->isTrafficLight() && !destinationNode->isGreen(streetId)) {
        continue;
      }
      destinationNode->addAgent(street->dequeue().value());
    }
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  void Dynamics<Id, Size, Delay>::m_evolveNodes(bool reinsert_agents) {
    /* In this function we have to manage the priority of the agents, given the street angles.
    By doing the angle difference, if the destination street is the same we can basically compare these differences (mod(pi)!, i.e. delta % std::numbers::pi):
    the smaller goes first.
    Anyway, this is not trivial as it seems so I will leave it as a comment.*/
    for (auto& [nodeId, node] : this->m_graph.nodeSet()) {
      for (const auto [priority, agentId] : node->agents()) {
        auto& agent = m_agents[agentId];
        if (nodeId == this->m_itineraries[agent->itineraryId()]->destination()) {
          node->removeAgent(agentId);
          this->m_travelTimes.push_back(agent->time());
          if (reinsert_agents) {
            Agent<Id, Size, Delay> newAgent{agent->id(), agent->itineraryId()};
            if (agent->srcNodeId().has_value()) {
              newAgent.setSourceNodeId(agent->srcNodeId().value());
            }
            this->removeAgent(agentId);
            this->addAgent(newAgent);
          } else {
            this->removeAgent(agentId);
          }
          continue;
        }
        auto possibleMoves{
            this->m_itineraries[agent->itineraryId()]->path().getRow(nodeId, true)};
        if (this->m_uniformDist(this->m_generator) < this->m_errorProbability) {
          possibleMoves = this->m_graph.adjMatrix().getRow(node->id(), true);
        }
        if (static_cast<Size>(possibleMoves.size()) == 0) {
          continue;
        }
        std::uniform_int_distribution<Size> moveDist{
            0, static_cast<Size>(possibleMoves.size() - 1)};
        const auto p{moveDist(this->m_generator)};
        auto iterator = possibleMoves.begin();
        std::advance(iterator, p);

        auto& nextStreet{this->m_graph.streetSet()[iterator->first]};

        if (nextStreet->density() < 1) {
          node->removeAgent(agentId);
          agent->setStreetId(nextStreet->id());
          this->setAgentSpeed(agentId);
          agent->incrementDelay(std::ceil(nextStreet->length() / agent->speed()));
          nextStreet->enqueue(agentId);
        } else {
          break;
        }
      }
      if (node->isTrafficLight()) {
        node->increaseCounter();
      }
    }
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  void Dynamics<Id, Size, Delay>::m_evolveAgents() {
    for (const auto& [agentId, agent] : this->m_agents) {
      if (agent->time() > 0) {
        if (agent->delay() > 0) {
          if (agent->delay() > 1) {
            agent->incrementDistance();
          } else if (agent->streetId().has_value()) {
            double distance{
                std::fmod(this->m_graph.streetSet()[agent->streetId().value()]->length(),
                          agent->speed())};
            if (distance < std::numeric_limits<double>::epsilon()) {
              agent->incrementDistance();
            } else {
              agent->incrementDistance(distance);
            }
          }
          agent->decrementDelay();
        }
      } else if (!agent->streetId().has_value()) {
        assert(agent->srcNodeId().has_value());
        const auto& srcNode{this->m_graph.nodeSet()[agent->srcNodeId().value()]};
        if (srcNode->isFull()) {
          continue;
        }
        try {
          srcNode->addAgent(agentId);
        } catch (std::runtime_error& e) {
          std::cerr << e.what() << '\n';
          continue;
        }
      }
      agent->incrementTime();
    }
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  void Dynamics<Id, Size, Delay>::setItineraries(std::span<Itinerary<Id>> itineraries) {
    std::ranges::for_each(itineraries, [this](const auto& itinerary) {
      this->m_itineraries.insert(std::make_unique<Itinerary<Id>>(itinerary));
    });
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  void Dynamics<Id, Size, Delay>::setMinSpeedRateo(double minSpeedRateo) {
    if (minSpeedRateo < 0. || minSpeedRateo > 1.) {
      throw std::invalid_argument(
          buildLog("The minim speed rateo must be between 0 and 1"));
    }
    m_minSpeedRateo = minSpeedRateo;
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  void Dynamics<Id, Size, Delay>::setErrorProbability(double errorProbability) {
    if (errorProbability < 0. || errorProbability > 1.) {
      throw std::invalid_argument(
          buildLog("The error probability must be between 0 and 1"));
    }
    m_errorProbability = errorProbability;
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  void Dynamics<Id, Size, Delay>::updatePaths() {
    const Size dimension = m_graph.adjMatrix().getRowDim();
    for (const auto& [itineraryId, itinerary] : m_itineraries) {
      SparseMatrix<Id, bool> path{dimension, dimension};
      // cycle over the nodes
      for (Size i{0}; i < dimension; ++i) {
        if (i == itinerary->destination()) {
          continue;
        }
        auto result{m_graph.shortestPath(i, itinerary->destination())};
        if (!result.has_value()) {
          continue;
        }
        // save the minimum distance between i and the destination
        const auto minDistance{result.value().distance()};
        for (const auto& node : m_graph.adjMatrix().getRow(i)) {
          // init distance from a neighbor node to the destination to zero
          double distance{0.};

          // can't dereference because risk undefined behavior
          auto streetResult = m_graph.street(i, node.first);
          if (streetResult == nullptr) {
            continue;
          }
          auto streetLength{(*streetResult)->length()};
          // TimePoint expectedTravelTime{
          //     streetLength};  // / street->maxSpeed()};  // TODO: change into input velocity
          result = m_graph.shortestPath(node.first, itinerary->destination());
          if (result.has_value()) {
            // if the shortest path exists, save the distance
            distance = result.value().distance();
          } else if (node.first != itinerary->destination()) {
            // if the node is the destination, the distance is zero, otherwise the iteration is skipped
            continue;
          }

          // if (!(distance > minDistance + expectedTravelTime)) {
          if (minDistance == distance + streetLength) {
            // std::cout << "minDistance: " << minDistance << " distance: " << distance
            //           << " streetLength: " << streetLength << '\n';
            // std::cout << "Inserting " << i << ';' << node.first << '\n';
            path.insert(i, node.first, true);
          }
        }
        itinerary->setPath(path);
      }
    }
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
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
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  const std::unordered_map<Id, std::unique_ptr<Itinerary<Id>>>&
  Dynamics<Id, Size, Delay>::itineraries() const {
    return m_itineraries;
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  const std::map<Id, std::unique_ptr<Agent<Id, Size, Delay>>>&
  Dynamics<Id, Size, Delay>::agents() const {
    return this->m_agents;
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  TimePoint Dynamics<Id, Size, Delay>::time() const {
    return m_time;
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  void Dynamics<Id, Size, Delay>::addAgent(const Agent<Id, Size, Delay>& agent) {
    if (this->m_agents.contains(agent.id())) {
      throw std::invalid_argument(
          buildLog("Agent " + std::to_string(agent.id()) + " already exists."));
    }
    this->m_agents.emplace(agent.id(), std::make_unique<Agent<Id, Size, Delay>>(agent));
  }
  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  void Dynamics<Id, Size, Delay>::addAgent(std::unique_ptr<Agent<Id, Size, Delay>> agent) {
    if (this->m_agents.contains(agent->id())) {
      throw std::invalid_argument(
          buildLog("Agent " + std::to_string(agent->id()) + " already exists."));
    }
    this->m_agents.emplace(agent->id(), std::move(agent));
  }
  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  void Dynamics<Id, Size, Delay>::addAgents(Id itineraryId,
                                            Size nAgents,
                                            std::optional<Id> srcNodeId) {
    auto itineraryIt{m_itineraries.find(itineraryId)};
    if (itineraryIt == m_itineraries.end()) {
      throw std::invalid_argument(
          buildLog("Itinerary " + std::to_string(itineraryId) + " not found"));
    }
    Size agentId{0};
    if (!this->m_agents.empty()) {
      agentId = this->m_agents.rbegin()->first + 1;
    }
    for (auto i{0}; i < nAgents; ++i, ++agentId) {
      this->addAgent(Agent<Id, Size, Delay>{agentId, itineraryId});
      if (srcNodeId.has_value()) {
        this->m_agents[agentId]->setSourceNodeId(srcNodeId.value());
      }
    }
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  template <typename... Tn>
    requires(is_agent_v<Tn> && ...)
  void Dynamics<Id, Size, Delay>::addAgents(Tn... agents) {}

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  template <typename T1, typename... Tn>
    requires(is_agent_v<T1> && (is_agent_v<Tn> && ...))
  void Dynamics<Id, Size, Delay>::addAgents(T1 agent, Tn... agents) {
    addAgent(agent);
    addAgents(agents...);
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  void Dynamics<Id, Size, Delay>::addAgents(std::span<Agent<Id, Size, Delay>> agents) {
    std::ranges::for_each(agents, [this](const auto& agent) -> void {
      this->m_agents.push_back(std::make_unique(agent));
    });
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  void Dynamics<Id, Size, Delay>::addAgentsUniformly(Size nAgents,
                                                     std::optional<Id> itineraryId) {
    if (this->m_itineraries.empty()) {
      // TODO: make this possible for random agents
      throw std::runtime_error(
          buildLog("It is not possible to add random agents without itineraries."));
    }
    const bool randomItinerary{!itineraryId.has_value()};
    std::uniform_int_distribution<Size> itineraryDist{
        0, static_cast<Size>(this->m_itineraries.size() - 1)};
    std::uniform_int_distribution<Size> streetDist{
        0, static_cast<Size>(this->m_graph.streetSet().size() - 1)};
    for (Size i{0}; i < nAgents; ++i) {
      if (randomItinerary) {
        itineraryId = itineraryDist(this->m_generator);
      }
      Id agentId{0};
      if (!this->m_agents.empty()) {
        agentId = this->m_agents.rbegin()->first + 1;
      }
      Id streetId{0};
      do {
        // I dunno why this works and the following doesn't
        const auto& streetSet = this->m_graph.streetSet();
        auto streetIt = streetSet.begin();
        // auto streetIt = this->m_graph->streetSet().begin();
        Size step = streetDist(this->m_generator);
        std::advance(streetIt, step);
        streetId = streetIt->first;
      } while (this->m_graph.streetSet()[streetId]->density() == 1);
      auto& street{this->m_graph.streetSet()[streetId]};
      Agent<Id, Size, Delay> agent{
          agentId, itineraryId.value(), street->nodePair().first};
      agent.setStreetId(streetId);
      this->addAgent(agent);
      this->setAgentSpeed(agentId);
      this->m_agents[agentId]->incrementDelay(
          std::ceil(street->length() / this->m_agents[agentId]->speed()));
      street->enqueue(agentId);
      ++agentId;
    }
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  void Dynamics<Id, Size, Delay>::removeAgent(Size agentId) {
    auto agentIt{m_agents.find(agentId)};
    if (agentIt == m_agents.end()) {
      throw std::invalid_argument(
          buildLog("Agent " + std::to_string(agentId) + " not found."));
    }
    m_agents.erase(agentId);
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  template <typename T1, typename... Tn>
    requires(std::is_convertible_v<T1, Size> && (std::is_convertible_v<Tn, Size> && ...))
  void Dynamics<Id, Size, Delay>::removeAgents(T1 id, Tn... ids) {
    removeAgent(id);
    removeAgents(ids...);
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  void Dynamics<Id, Size, Delay>::addItinerary(const Itinerary<Id>& itinerary) {
    m_itineraries.emplace(itinerary.id(), std::make_unique<Itinerary<Id>>(itinerary));
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  void Dynamics<Id, Size, Delay>::addItinerary(std::unique_ptr<Itinerary<Id>> itinerary) {
    m_itineraries.emplace(itinerary.id(), std::move(itinerary));
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  template <typename... Tn>
    requires(is_itinerary_v<Tn> && ...)
  void Dynamics<Id, Size, Delay>::addItineraries(Tn... itineraries) {
    (this->addItinerary(itineraries), ...);
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  void Dynamics<Id, Size, Delay>::addItineraries(std::span<Itinerary<Id>> itineraries) {
    std::ranges::for_each(itineraries, [this](const auto& itinerary) -> void {
      this->m_itineraries.push_back(std::make_unique<Itinerary<Id>>(itinerary));
    });
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  void Dynamics<Id, Size, Delay>::resetTime() {
    m_time = 0;
  }

  // template <typename Id, typename Size, typename Delay>
  //   requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
  //            is_numeric_v<Delay>)
  // template <typename F, typename... Tn>
  //   requires(std::is_invocable_v<F, Tn...>)
  // void Dynamics<Id, Size, Delay>::evolve(F f, Tn... args) {
  //   f(args...);
  // }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  Measurement<double> Dynamics<Id, Size, Delay>::meanSpeed() const {
    if (m_agents.size() == 0) {
      return Measurement(0., 0.);
    }
    const double mean{std::accumulate(m_agents.cbegin(),
                                      m_agents.cend(),
                                      0.,
                                      [](double sum, const auto& agent) {
                                        return sum + agent.second->speed();
                                      }) /
                      m_agents.size()};
    if (m_agents.size() == 1) {
      return Measurement(mean, 0.);
    }
    const double variance{
        std::accumulate(m_agents.cbegin(),
                        m_agents.cend(),
                        0.,
                        [mean](double sum, const auto& agent) {
                          return sum + std::pow(agent.second->speed() - mean, 2);
                        }) /
        (m_agents.size() - 1)};
    return Measurement(mean, std::sqrt(variance));
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  Measurement<double> Dynamics<Id, Size, Delay>::streetMeanDensity() const {
    if (m_graph.streetSet().size() == 0) {
      return Measurement(0., 0.);
    }
    const double mean{std::accumulate(m_graph.streetSet().cbegin(),
                                      m_graph.streetSet().cend(),
                                      0.,
                                      [](double sum, const auto& street) {
                                        return sum + street.second->density();
                                      }) /
                      m_graph.streetSet().size()};
    const double variance{
        std::accumulate(m_graph.streetSet().cbegin(),
                        m_graph.streetSet().cend(),
                        0.,
                        [mean](double sum, const auto& street) {
                          return sum + std::pow(street.second->density() - mean, 2);
                        }) /
        (m_graph.streetSet().size() - 1)};
    return Measurement(mean, std::sqrt(variance));
  }
  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  Measurement<double> Dynamics<Id, Size, Delay>::streetMeanFlow() const {
    std::vector<double> flows;
    flows.reserve(m_graph.streetSet().size());
    for (const auto& [streetId, street] : m_graph.streetSet()) {
      auto speedOpt{this->streetMeanSpeed(streetId)};
      if (speedOpt.has_value()) {
        double flow{street->density() * speedOpt.value()};
        flows.push_back(flow);
      } else {
        flows.push_back(street->density());  // 0 * NaN = 0
      }
    }
    return Measurement(flows);
  }
  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  Measurement<double> Dynamics<Id, Size, Delay>::streetMeanFlow(double threshold,
                                                                bool above) const {
    std::vector<double> flows;
    flows.reserve(m_graph.streetSet().size());
    for (const auto& [streetId, street] : m_graph.streetSet()) {
      if (above && street->density() > threshold) {
        auto speedOpt{this->streetMeanSpeed(streetId)};
        if (speedOpt.has_value()) {
          double flow{street->density() * speedOpt.value()};
          flows.push_back(flow);
        } else {
          flows.push_back(street->density());  // 0 * NaN = 0
        }
      } else if (!above && street->density() < threshold) {
        auto speedOpt{this->streetMeanSpeed(streetId)};
        if (speedOpt.has_value()) {
          double flow{street->density() * speedOpt.value()};
          flows.push_back(flow);
        } else {
          flows.push_back(street->density());  // 0 * NaN = 0
        }
      }
    }
    return Measurement(flows);
  }
  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  Measurement<double> Dynamics<Id, Size, Delay>::meanTravelTime(bool clearData) {
    if (m_travelTimes.size() == 0) {
      return Measurement(0., 0.);
    }
    const double mean{std::accumulate(m_travelTimes.cbegin(), m_travelTimes.cend(), 0.) /
                      m_travelTimes.size()};
    const double variance{std::accumulate(m_travelTimes.cbegin(),
                                          m_travelTimes.cend(),
                                          0.,
                                          [mean](double sum, const auto& travelTime) {
                                            return sum + std::pow(travelTime - mean, 2);
                                          }) /
                          (m_travelTimes.size() - 1)};
    if (clearData) {
      m_travelTimes.clear();
    }
    return Measurement(mean, std::sqrt(variance));
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             std::unsigned_integral<Delay>)
  class FirstOrderDynamics : public Dynamics<Id, Size, Delay> {
  public:
    FirstOrderDynamics() = delete;
    /// @brief Construct a new First Order Dynamics object
    /// @param graph, The graph representing the network
    FirstOrderDynamics(Graph<Id, Size>& graph) : Dynamics<Id, Size, Delay>(graph){};
    /// @brief Set the speed of an agent
    /// @param agentId The id of the agent
    /// @throw std::invalid_argument, If the agent is not found
    void setAgentSpeed(Size agentId) override;
    /// @brief Get the mean speed of a street
    /// @details The mean speed of a street is given by the formula:
    /// \f$ v_{\text{mean}} = v_{\text{max}} \left(1 - \frac{\alpha}{2} \left( n - 1\right)  \right) \f$
    /// where \f$ v_{\text{max}} \f$ is the maximum speed of the street, \f$ \alpha \f$ is the minimum speed rateo divided by the capacity
    /// and \f$ n \f$ is the number of agents in the street
    std::optional<double> streetMeanSpeed(Id streetId) const override;
    /// @brief Get the mean speed of the streets
    /// @return Measurement The mean speed of the agents and the standard deviation
    Measurement<double> streetMeanSpeed() const override;
    /// @brief Get the mean speed of the streets
    /// @param threshold The density threshold to consider
    /// @param above If true, the function returns the mean speed of the streets with a density above the threshold, otherwise below
    /// @return Measurement The mean speed of the agents and the standard deviation
    Measurement<double> streetMeanSpeed(double threshold, bool above) const override;
  };

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             std::unsigned_integral<Delay>)
  void FirstOrderDynamics<Id, Size, Delay>::setAgentSpeed(Size agentId) {
    const auto& street{
        this->m_graph.streetSet()[this->m_agents[agentId]->streetId().value()]};
    double speed{street->maxSpeed() * (1. - this->m_minSpeedRateo * street->density())};
    this->m_agents[agentId]->setSpeed(speed);
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             std::unsigned_integral<Delay>)
  std::optional<double> FirstOrderDynamics<Id, Size, Delay>::streetMeanSpeed(
      Id streetId) const {
    const auto& street{this->m_graph.streetSet().at(streetId)};
    if (street->queue().empty()) {
      return std::nullopt;
    }
    double meanSpeed{0.};
    Size n{0};
    if (this->m_agents.at(street->queue().front())->delay() > 0) {
      n = static_cast<Size>(street->queue().size());
      double alpha{this->m_minSpeedRateo / street->capacity()};
      meanSpeed = street->maxSpeed() * n * (1. - 0.5 * alpha * (n - 1.));
    } else {
      for (const auto& agentId : street->queue()) {
        meanSpeed += this->m_agents.at(agentId)->speed();
        ++n;
      }
    }

    for (const auto& [angle, agentId] :
         this->m_graph.nodeSet().at(street->nodePair().second)->agents()) {
      const auto& agent{this->m_agents.at(agentId)};
      if (agent->streetId().has_value() && agent->streetId().value() == streetId) {
        meanSpeed += agent->speed();
        ++n;
      }
    }
    return meanSpeed / n;
  }

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             std::unsigned_integral<Delay>)
  Measurement<double> FirstOrderDynamics<Id, Size, Delay>::streetMeanSpeed() const {
    if (this->m_agents.size() == 0) {
      return Measurement(0., 0.);
    }
    std::vector<double> speeds;
    speeds.reserve(this->m_graph.streetSet().size());
    for (const auto& [streetId, street] : this->m_graph.streetSet()) {
      auto speedOpt{this->streetMeanSpeed(streetId)};
      if (speedOpt.has_value()) {
        speeds.push_back(speedOpt.value());
      }
    }
    return Measurement(speeds);
  }
  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             std::unsigned_integral<Delay>)
  Measurement<double> FirstOrderDynamics<Id, Size, Delay>::streetMeanSpeed(
      double threshold, bool above) const {
    if (this->m_agents.size() == 0) {
      return Measurement(0., 0.);
    }
    std::vector<double> speeds;
    speeds.reserve(this->m_graph.streetSet().size());
    for (const auto& [streetId, street] : this->m_graph.streetSet()) {
      if (above) {
        if (street->density() > threshold) {
          auto speedOpt{this->streetMeanSpeed(streetId)};
          if (speedOpt.has_value()) {
            speeds.push_back(speedOpt.value());
          }
        }
      } else {
        if (street->density() < threshold) {
          auto speedOpt{this->streetMeanSpeed(streetId)};
          if (speedOpt.has_value()) {
            speeds.push_back(speedOpt.value());
          }
        }
      }
    }
    return Measurement(speeds);
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
