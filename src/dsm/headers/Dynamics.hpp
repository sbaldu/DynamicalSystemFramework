/// @file       /src/dsm/headers/Dynamics.hpp
/// @brief      Defines the Dynamics class.
///
/// @details    This file contains the definition of the Dynamics class.
///             The Dynamics class represents the dynamics of the network. It is templated by the type
///             of the graph's id and the type of the graph's capacity.
///             The graph's id and capacity must be unsigned integral types.

#pragma once

#include <algorithm>
#include <concepts>
#include <vector>
#include <random>
#include <span>
#include <numeric>
#include <unordered_map>
#include <cmath>
#include <cassert>
#include <format>
#include <thread>

#include "Agent.hpp"
#include "Itinerary.hpp"
#include "Graph.hpp"
#include "SparseMatrix.hpp"
#include "../utility/TypeTraits/is_agent.hpp"
#include "../utility/TypeTraits/is_itinerary.hpp"
#include "../utility/Logger.hpp"
#include "../utility/Typedef.hpp"

namespace dsm {

  using TimePoint = long long unsigned int;

  /// @brief The Measurement struct represents the mean of a quantity and its standard deviation
  /// @tparam T The type of the mean and the standard deviation
  /// @param mean The mean
  /// @param std The standard deviation of the sample
  template <typename T>
  struct Measurement {
    T mean;
    T std;

    Measurement(T mean, T std) : mean{mean}, std{std} {}
    Measurement(const std::vector<T>& data) {
      if (data.size() == 0) {
        mean = 0.;
        std = 0.;
      } else {
        mean = std::accumulate(data.cbegin(), data.cend(), 0.) / data.size();
        if (data.size() < 2) {
          std = 0.;
        } else {
          const double cvariance{std::accumulate(data.cbegin(),
                                                 data.cend(),
                                                 0.,
                                                 [this](double sum, const auto& value) {
                                                   return sum + std::pow(value - mean, 2);
                                                 }) /
                                 (data.size() - 1)};
          std = std::sqrt(cvariance);
        }
      }
    }
  };

  /// @brief The Dynamics class represents the dynamics of the network.
  /// @tparam Id, The type of the graph's id. It must be an unsigned integral type.
  /// @tparam Size, The type of the graph's capacity. It must be an unsigned integral type.
  template <typename Delay>
    requires(is_numeric_v<Delay>)
  class Dynamics {
  protected:
    std::unordered_map<Id, std::unique_ptr<Itinerary>> m_itineraries;
    std::map<Id, std::unique_ptr<Agent<Delay>>> m_agents;
    TimePoint m_time, m_previousSpireTime;
    Graph m_graph;
    double m_errorProbability;
    double m_minSpeedRateo;
    double m_maxFlowPercentage;
    mutable std::mt19937_64 m_generator{std::random_device{}()};
    std::uniform_real_distribution<double> m_uniformDist{0., 1.};
    std::vector<unsigned int> m_travelTimes;
    std::unordered_map<Id, Id> m_agentNextStreetId;
    bool m_forcePriorities;
    std::optional<Delay> m_dataUpdatePeriod;
    std::unordered_map<Id, std::array<unsigned long long, 4>> m_turnCounts;
    std::unordered_map<Id, std::array<long, 4>> m_turnMapping;
    std::unordered_map<Id, unsigned long long> m_streetTails;

    /// @brief Get the next street id
    /// @param agentId The id of the agent
    /// @param NodeId The id of the node
    /// @param streetId The id of the incoming street
    /// @return Id The id of the randomly selected next street
    virtual Id m_nextStreetId(Id agentId,
                              Id NodeId,
                              std::optional<Id> streetId = std::nullopt);
    /// @brief Increase the turn counts
    virtual void m_increaseTurnCounts(Id streetId, double delta);
    /// @brief Evolve the streets
    /// @param reinsert_agents If true, the agents are reinserted in the simulation after they reach their destination
    /// @details If possible, removes the first agent of each street queue, putting it in the destination node.
    /// If the agent is going into the destination node, it is removed from the simulation (and then reinserted if reinsert_agents is true)
    virtual void m_evolveStreets(bool reinsert_agents);
    /// @brief Evolve the nodes
    /// @details If possible, removes all agents from each node, putting them on the next street.
    /// If the error probability is not zero, the agents can move to a random street.
    virtual void m_evolveNodes();
    /// @brief Evolve the agents.
    /// @details Puts all new agents on a street, if possible, decrements all delays
    /// and increments all travel times.
    virtual void m_evolveAgents();
    /// @brief Update the path of a single itinerary
    /// @param pItinerary An std::unique_prt to the itinerary
    void m_updatePath(const std::unique_ptr<Itinerary>& pItinerary) {
      const Size dimension = m_graph.adjMatrix().getRowDim();
      const auto destinationID = pItinerary->destination();
      SparseMatrix<bool> path{dimension, dimension};
      // cycle over the nodes
      for (const auto& [nodeId, node] : m_graph.nodeSet()) {
        if (nodeId == destinationID) {
          continue;
        }
        auto result{m_graph.shortestPath(nodeId, destinationID)};
        if (!result.has_value()) {
          continue;
        }
        // save the minimum distance between i and the destination
        const auto minDistance{result.value().distance()};
        for (const auto [nextNodeId, _] : m_graph.adjMatrix().getRow(nodeId)) {
          if (nextNodeId == destinationID &&
              minDistance ==
                  m_graph.streetSet().at(nodeId * dimension + nextNodeId)->length()) {
            path.insert(nodeId, nextNodeId, true);
            continue;
          }
          result = m_graph.shortestPath(nextNodeId, destinationID);

          if (result.has_value()) {
            // if the shortest path exists, save the distance
            if (minDistance ==
                result.value().distance() +
                    m_graph.streetSet().at(nodeId * dimension + nextNodeId)->length()) {
              path.insert(nodeId, nextNodeId, true);
            }
          } else if ((nextNodeId != destinationID)) {
            std::cerr << std::format("WARNING: No path found from node {} to node {}",
                                     nextNodeId,
                                     destinationID)
                      << std::endl;
          }
        }
      }
      if (path.size() == 0) {
        throw std::runtime_error(
            buildLog(std::format("Path with id {} and destination {} is empty. Please "
                                 "check the adjacency matrix.",
                                 pItinerary->id(),
                                 pItinerary->destination())));
      }
      pItinerary->setPath(path);
    }

  public:
    /// @brief Construct a new Dynamics object
    /// @param graph The graph representing the network
    Dynamics(Graph& graph);

    /// @brief Set the itineraries
    /// @param itineraries The itineraries
    void setItineraries(std::span<Itinerary> itineraries);
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
    /// @brief Set the maximum flow percentage
    /// @param maxFlowPercentage The maximum flow percentage
    /// @details The maximum flow percentage is the percentage of the maximum flow that a street can transmit. Default is 1 (100%).
    /// @throw std::invalid_argument If the maximum flow percentage is not between 0 and 1
    void setMaxFlowPercentage(double maxFlowPercentage);
    /// @brief Set the dynamics destination nodes auto-generating itineraries
    /// @param destinationNodes The destination nodes
    /// @param updatePaths If true, the paths are updated
    /// @throws std::invalid_argument Ifone or more destination nodes do not exist
    void setDestinationNodes(const std::span<Id>& destinationNodes,
                             bool updatePaths = true);
    /// @brief Set the speed of an agent
    /// @details This is a pure-virtual function, it must be implemented in the derived classes
    /// @param agentId The id of the agent
    virtual void setAgentSpeed(Size agentId) = 0;
    /// @brief Set the force priorities flag
    /// @param forcePriorities The flag
    /// @details If true, if an agent cannot move to the next street, the whole node is skipped
    void setForcePriorities(bool forcePriorities) { m_forcePriorities = forcePriorities; }
    /// @brief Set the data update period.
    /// @param dataUpdatePeriod Delay, The period
    /// @details Some data, i.e. the street queue lengths, are stored only after a fixed amount of time which is represented by this variable.
    void setDataUpdatePeriod(Delay dataUpdatePeriod) {
      m_dataUpdatePeriod = dataUpdatePeriod;
    }

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
    /// @brief Optimize the traffic lights by changing the green and red times
    /// @param nCycles Delay, The number of cycles (times agents are being added) between two calls of this function
    /// @param threshold double, The percentage of the mean capacity of the streets used as threshold for the delta between the two tails.
    /// @param densityTolerance double, The algorithm will consider all streets with density up to densityTolerance*meanDensity
    /// @details The function cycles over the traffic lights and, if the difference between the two tails is greater than
    ///   the threshold multiplied by the mean capacity of the streets, it changes the green and red times of the traffic light, keeping the total cycle time constant.
    void optimizeTrafficLights(Delay nCycles,
                               double threshold = 0.,
                               double densityTolerance = 0.);

    /// @brief Get the graph
    /// @return const Graph&, The graph
    const Graph& graph() const { return m_graph; };
    /// @brief Get the itineraries
    /// @return const std::unordered_map<Id, Itinerary>&, The itineraries
    const std::unordered_map<Id, std::unique_ptr<Itinerary>>& itineraries() const {
      return m_itineraries;
    }
    /// @brief Get the agents
    /// @return const std::unordered_map<Id, Agent<Id>>&, The agents
    const std::map<Id, std::unique_ptr<Agent<Delay>>>& agents() const { return m_agents; }
    /// @brief Get the time
    /// @return TimePoint The time
    TimePoint time() const { return m_time; }

    /// @brief Add an agent to the simulation
    /// @param agent The agent
    void addAgent(const Agent<Delay>& agent);
    /// @brief Add an agent to the simulation
    /// @param agent std::unique_ptr to the agent
    void addAgent(std::unique_ptr<Agent<Delay>> agent);
    /// @brief Add an agent with given source node and itinerary
    /// @param srcNodeId The id of the source node
    /// @param itineraryId The id of the itinerary
    /// @throws std::invalid_argument If the source node or the itinerary are not found
    void addAgent(Id srcNodeId, Id itineraryId);
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
    void addAgents(std::span<Agent<Delay>> agents);
    /// @brief Add a set of agents to the simulation
    /// @param nAgents The number of agents to add
    /// @param uniformly If true, the agents are added uniformly on the streets
    /// @throw std::runtime_error If there are no itineraries
    virtual void addAgentsUniformly(Size nAgents,
                                    std::optional<Id> itineraryId = std::nullopt);
    template <typename TContainer>
      requires(std::is_same_v<TContainer, std::unordered_map<Id, double>> ||
               std::is_same_v<TContainer, std::map<Id, double>>)
    void addAgentsRandomly(Size nAgents,
                           const TContainer& src_weights,
                           const TContainer& dst_weights);

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
    void addItinerary(const Itinerary& itinerary);
    /// @brief Add an itinerary
    /// @param itinerary std::unique_ptr to the itinerary
    void addItinerary(std::unique_ptr<Itinerary> itinerary);
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
    void addItineraries(std::span<Itinerary> itineraries);

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

    /// @brief Get the mean speed of the agents in \f$m/s\f$
    /// @return Measurement<double> The mean speed of the agents and the standard deviation
    Measurement<double> agentMeanSpeed() const;
    // TODO: implement the following functions
    // We can implement the base version of these functions by cycling over agents... I won't do it for now.
    // Grufoony - 19/02/2024
    virtual double streetMeanSpeed(Id) const = 0;
    virtual Measurement<double> streetMeanSpeed() const = 0;
    virtual Measurement<double> streetMeanSpeed(double, bool) const = 0;
    /// @brief Get the mean density of the streets in \f$m^{-1}\f$
    /// @return Measurement<double> The mean density of the streets and the standard deviation
    Measurement<double> streetMeanDensity() const;
    /// @brief Get the mean flow of the streets in \f$s^{-1}\f$
    /// @return Measurement<double> The mean flow of the streets and the standard deviation
    Measurement<double> streetMeanFlow() const;
    /// @brief Get the mean flow of the streets in \f$s^{-1}\f$
    /// @param threshold The density threshold to consider
    /// @param above If true, the function returns the mean flow of the streets with a density above the threshold, otherwise below
    /// @return Measurement<double> The mean flow of the streets and the standard deviation
    Measurement<double> streetMeanFlow(double threshold, bool above) const;
    /// @brief Get the mean spire input flow of the streets in \f$s^{-1}\f$
    /// @param resetValue If true, the spire input/output flows are cleared after the computation
    /// @return Measurement<double> The mean spire input flow of the streets and the standard deviation
    /// @details The spire input flow is computed as the sum of counts over the product of the number of spires and the time delta
    Measurement<double> meanSpireInputFlow(bool resetValue = true);
    /// @brief Get the mean spire output flow of the streets in \f$s^{-1}\f$
    /// @param resetValue If true, the spire output/input flows are cleared after the computation
    /// @return Measurement<double> The mean spire output flow of the streets and the standard deviation
    /// @details The spire output flow is computed as the sum of counts over the product of the number of spires and the time delta
    Measurement<double> meanSpireOutputFlow(bool resetValue = true);
    /// @brief Get the mean travel time of the agents in \f$s\f$
    /// @param clearData If true, the travel times are cleared after the computation
    /// @return Measurement<double> The mean travel time of the agents and the standard
    Measurement<double> meanTravelTime(bool clearData = false);
    /// @brief Get the turn counts of the agents
    /// @return const std::array<unsigned long long, 3>& The turn counts
    /// @details The array contains the counts of left (0), straight (1), right (2) and U (3) turns
    const std::unordered_map<Id, std::array<unsigned long long, 4>>& turnCounts() const {
      return m_turnCounts;
    }
    /// @brief Get the turn probabilities of the agents
    /// @return std::array<double, 3> The turn probabilities
    /// @details The array contains the probabilities of left (0), straight (1), right (2) and U (3) turns
    std::unordered_map<Id, std::array<double, 4>> turnProbabilities(bool reset = true);

    std::unordered_map<Id, std::array<long, 4>> turnMapping() const {
      return m_turnMapping;
    }
  };

  template <typename Delay>
    requires(is_numeric_v<Delay>)
  Dynamics<Delay>::Dynamics(Graph& graph)
      : m_time{0},
        m_previousSpireTime{0},
        m_graph{std::move(graph)},
        m_errorProbability{0.},
        m_minSpeedRateo{0.},
        m_maxFlowPercentage{1.},
        m_forcePriorities{false} {
    for (const auto& [streetId, street] : m_graph.streetSet()) {
      m_streetTails.emplace(streetId, 0);
      m_turnCounts.emplace(streetId, std::array<unsigned long long, 4>{0, 0, 0, 0});
      // fill turn mapping as [streetId, [left street Id, straight street Id, right street Id, U self street Id]]
      m_turnMapping.emplace(streetId, std::array<long, 4>{-1, -1, -1, -1});
    }
    for (const auto& [streetId, street] : m_graph.streetSet()) {
      const auto& srcNodeId = street->nodePair().second;
      for (const auto& [ss, _] : m_graph.adjMatrix().getRow(srcNodeId, true)) {
        // const auto& nextStreet = m_graph.streetSet()[ss];
        // if (nextStreet == nullptr) {
        //   std::cout << "Street " << ss << " not found\n";
        //   continue;
        // }
        const auto& delta = street->angle() - m_graph.streetSet()[ss]->angle();
        if (std::abs(delta) < std::numbers::pi) {
          if (delta < 0.) {
            m_turnMapping[streetId][0] = ss;
            ;  // right
          } else if (delta > 0.) {
            m_turnMapping[streetId][2] = ss;  // left
          } else {
            m_turnMapping[streetId][1] = ss;  // straight
          }
        } else {
          m_turnMapping[streetId][3] = ss;  // U
        }
      }
    }
  }

  template <typename Delay>
    requires(is_numeric_v<Delay>)
  Id Dynamics<Delay>::m_nextStreetId(Id agentId, Id nodeId, std::optional<Id> streetId) {
    auto possibleMoves = m_graph.adjMatrix().getRow(nodeId, true);
    if (this->m_itineraries.size() > 0 &&
        this->m_uniformDist(this->m_generator) > this->m_errorProbability) {
      const auto& it = this->m_itineraries[this->m_agents[agentId]->itineraryId()];
      if (it->destination() != nodeId) {
        possibleMoves = it->path().getRow(nodeId, true);
      }
    }
    assert(possibleMoves.size() > 0);
    std::uniform_int_distribution<Size> moveDist{
        0, static_cast<Size>(possibleMoves.size() - 1)};
    uint8_t p{0};
    auto iterator = possibleMoves.begin();
    // while loop to avoid U turns in non-roundabout junctions
    do {
      p = moveDist(this->m_generator);
      iterator = possibleMoves.begin();
      std::advance(iterator, p);
    } while (!m_graph.nodeSet().at(nodeId)->isRoundabout() and streetId.has_value() and
             (m_graph.streetSet()[iterator->first]->nodePair().second ==
              m_graph.streetSet()[streetId.value()]->nodePair().first) and
             (possibleMoves.size() > 1));
    return iterator->first;
  }

  template <typename Delay>
    requires(is_numeric_v<Delay>)
  void Dynamics<Delay>::m_increaseTurnCounts(Id streetId, double delta) {
    if (std::abs(delta) < std::numbers::pi) {
      if (delta < 0.) {
        ++m_turnCounts[streetId][0];  // right
      } else if (delta > 0.) {
        ++m_turnCounts[streetId][2];  // left
      } else {
        ++m_turnCounts[streetId][1];  // straight
      }
    } else {
      ++m_turnCounts[streetId][3];  // U
    }
  }

  template <typename Delay>
    requires(is_numeric_v<Delay>)
  void Dynamics<Delay>::m_evolveStreets(bool reinsert_agents) {
    for (const auto& [streetId, street] : m_graph.streetSet()) {
      if (m_uniformDist(m_generator) > m_maxFlowPercentage || street->queue().empty()) {
        continue;
      }
      const auto agentId{street->queue().front()};
      if (m_agents[agentId]->delay() > 0) {
        continue;
      }
      if (m_dataUpdatePeriod.has_value()) {
        if (m_time % m_dataUpdatePeriod.value() == 0) {
          //m_streetTails[streetId] += street->queue().size();
          m_streetTails[streetId] += street->waitingAgents().size();
        }
      }
      m_agents[agentId]->setSpeed(0.);
      const auto& destinationNode{this->m_graph.nodeSet()[street->nodePair().second]};
      if (destinationNode->isFull()) {
        continue;
      }
      if (destinationNode->isTrafficLight()) {
        auto& tl = dynamic_cast<TrafficLight<Delay>&>(*destinationNode);
        if (!tl.isGreen(streetId)) {
          continue;
        }
      }
      if (destinationNode->id() ==
          m_itineraries[m_agents[agentId]->itineraryId()]->destination()) {
        street->dequeue();
        m_travelTimes.push_back(m_agents[agentId]->time());
        if (reinsert_agents) {
          // take last agent id in map
          Agent<Delay> newAgent{static_cast<Id>(m_agents.rbegin()->first + 1),
                                m_agents[agentId]->itineraryId(),
                                m_agents[agentId]->srcNodeId().value()};
          if (m_agents[agentId]->srcNodeId().has_value()) {
            newAgent.setSourceNodeId(this->m_agents[agentId]->srcNodeId().value());
          }
          this->removeAgent(agentId);
          this->addAgent(newAgent);
        } else {
          this->removeAgent(agentId);
        }
        continue;
      }
      const auto& nextStreet{m_graph.streetSet()[this->m_nextStreetId(
          agentId, destinationNode->id(), streetId)]};
      if (nextStreet->isFull()) {
        continue;
      }
      street->dequeue();
      assert(destinationNode->id() == nextStreet->nodePair().first);
      if (destinationNode->isIntersection()) {
        auto& intersection = dynamic_cast<Node&>(*destinationNode);
        auto delta = nextStreet->angle() - street->angle();
        if (delta > std::numbers::pi) {
          delta -= 2 * std::numbers::pi;
        } else if (delta < -std::numbers::pi) {
          delta += 2 * std::numbers::pi;
        }
        m_increaseTurnCounts(streetId, delta);
        intersection.addAgent(delta, agentId);
        m_agentNextStreetId.emplace(agentId, nextStreet->id());
      } else if (destinationNode->isRoundabout()) {
        auto& roundabout = dynamic_cast<Roundabout&>(*destinationNode);
        roundabout.enqueue(agentId);
      }
    }
  }

  template <typename Delay>
    requires(is_numeric_v<Delay>)
  void Dynamics<Delay>::m_evolveNodes() {
    for (const auto& [nodeId, node] : m_graph.nodeSet()) {
      if (node->isIntersection()) {
        auto& intersection = dynamic_cast<Node&>(*node);
        for (const auto [angle, agentId] : intersection.agents()) {
          const auto& nextStreet{m_graph.streetSet()[m_agentNextStreetId[agentId]]};
          if (!(nextStreet->isFull())) {
            intersection.removeAgent(agentId);
            m_agents[agentId]->setStreetId(nextStreet->id());
            this->setAgentSpeed(agentId);
            m_agents[agentId]->incrementDelay(
                std::ceil(nextStreet->length() / m_agents[agentId]->speed()));
            nextStreet->addAgent(agentId);
            m_agentNextStreetId.erase(agentId);
          } else if (m_forcePriorities) {
            break;
          }
        }
        if (node->isTrafficLight()) {
          auto& tl = dynamic_cast<TrafficLight<Delay>&>(*node);
          tl.increaseCounter();
        }
      } else if (node->isRoundabout()) {
        auto& roundabout = dynamic_cast<Roundabout&>(*node);
        const auto nAgents{roundabout.agents().size()};
        for (size_t i{0}; i < nAgents; ++i) {
          const auto agentId{roundabout.agents().front()};
          const auto& nextStreet{this->m_graph.streetSet()[m_nextStreetId(
              agentId, node->id(), m_agents[agentId]->streetId())]};
          if (!(nextStreet->isFull())) {
            if (m_agents[agentId]->streetId().has_value()) {
              const auto streetId = m_agents[agentId]->streetId().value();
              auto delta = nextStreet->angle() - m_graph.streetSet()[streetId]->angle();
              if (delta > std::numbers::pi) {
                delta -= 2 * std::numbers::pi;
              } else if (delta < -std::numbers::pi) {
                delta += 2 * std::numbers::pi;
              }
              m_increaseTurnCounts(streetId, delta);
            }
            roundabout.dequeue();
            m_agents[agentId]->setStreetId(nextStreet->id());
            this->setAgentSpeed(agentId);
            m_agents[agentId]->incrementDelay(
                std::ceil(nextStreet->length() / m_agents[agentId]->speed()));
            nextStreet->addAgent(agentId);
          } else {
            break;
          }
        }
      }
    }
  }

  template <typename Delay>
    requires(is_numeric_v<Delay>)
  void Dynamics<Delay>::m_evolveAgents() {
    for (const auto& [agentId, agent] : this->m_agents) {
      if (agent->delay() > 0) {
        const auto& street{m_graph.streetSet()[agent->streetId().value()]};
        if (agent->delay() > 1) {
          agent->incrementDistance();
        } else {
          double distance{std::fmod(street->length(), agent->speed())};
          if (distance < std::numeric_limits<double>::epsilon()) {
            agent->incrementDistance();
          } else {
            agent->incrementDistance(distance);
          }
        }
        agent->decrementDelay();
        if (agent->delay() == 0) {
          street->enqueue(agentId);
        }
      } else if (!agent->streetId().has_value()) {
        assert(agent->srcNodeId().has_value());
        const auto& srcNode{this->m_graph.nodeSet()[agent->srcNodeId().value()]};
        if (srcNode->isFull()) {
          continue;
        }
        const auto& nextStreet{
            m_graph.streetSet()[this->m_nextStreetId(agentId, srcNode->id())]};
        if (nextStreet->isFull()) {
          continue;
        }
        assert(srcNode->id() == nextStreet->nodePair().first);
        if (srcNode->isIntersection()) {
          auto& intersection = dynamic_cast<Node&>(*srcNode);
          try {
            intersection.addAgent(0., agentId);
            m_agentNextStreetId.emplace(agentId, nextStreet->id());
          } catch (...) {
            continue;
          }
        } else if (srcNode->isRoundabout()) {
          auto& roundabout = dynamic_cast<Roundabout&>(*srcNode);
          try {
            roundabout.enqueue(agentId);
          } catch (...) {
            continue;
          }
        }
      } else if (agent->delay() == 0) {
        agent->setSpeed(0.);
      }
      agent->incrementTime();
    }
  }

  /* template <typename Delay> */
  /*   requires(is_numeric_v<Delay>) */
  /* void Dynamics<Delay>::m_updatePath(const std::unique_ptr < Itinerary >> &pItinerary) { */
  /*   const Size dimension = m_graph.adjMatrix().getRowDim(); */
  /*   const auto destinationID = pItinerary->destination(); */
  /*   SparseMatrix<bool> path{dimension, dimension}; */
  /*   // cycle over the nodes */
  /*   for (const auto& [nodeId, node] : m_graph.nodeSet()) { */
  /*     if (nodeId == destinationID) { */
  /*       continue; */
  /*     } */
  /*     auto result{m_graph.shortestPath(nodeId, destinationID)}; */
  /*     if (!result.has_value()) { */
  /*       continue; */
  /*     } */
  /*     // save the minimum distance between i and the destination */
  /*     const auto minDistance{result.value().distance()}; */
  /*     for (const auto [nextNodeId, _] : m_graph.adjMatrix().getRow(nodeId)) { */
  /*       if (nextNodeId == destinationID && */
  /*           minDistance == */
  /*               m_graph.streetSet().at(nodeId * dimension + nextNodeId)->length()) { */
  /*         path.insert(nodeId, nextNodeId, true); */
  /*         continue; */
  /*       } */
  /*       result = m_graph.shortestPath(nextNodeId, destinationID); */

  /*       if (result.has_value()) { */
  /*         // if the shortest path exists, save the distance */
  /*         if (minDistance == */
  /*             result.value().distance() + */
  /*                 m_graph.streetSet().at(nodeId * dimension + nextNodeId)->length()) { */
  /*           path.insert(nodeId, nextNodeId, true); */
  /*         } */
  /*       } else if ((nextNodeId != destinationID)) { */
  /*         std::cerr << std::format("WARNING: No path found from node {} to node {}", */
  /*                                  nextNodeId, */
  /*                                  destinationID) */
  /*                   << std::endl; */
  /*       } */
  /*     } */
  /*   } */
  /*   if (path.size() == 0) { */
  /*     throw std::runtime_error( */
  /*         buildLog(std::format("Path with id {} and destination {} is empty. Please " */
  /*                              "check the adjacency matrix.", */
  /*                              pItinerary->id(), */
  /*                              pItinerary->destination()))); */
  /*   } */
  /*   pItinerary->setPath(path); */
  /* } */

  template <typename Delay>
    requires(is_numeric_v<Delay>)
  void Dynamics<Delay>::setItineraries(std::span<Itinerary> itineraries) {
    std::ranges::for_each(itineraries, [this](const auto& itinerary) {
      this->m_itineraries.insert(std::make_unique<Itinerary>(itinerary));
    });
  }

  template <typename Delay>
    requires(is_numeric_v<Delay>)
  void Dynamics<Delay>::setMinSpeedRateo(double minSpeedRateo) {
    if (minSpeedRateo < 0. || minSpeedRateo > 1.) {
      throw std::invalid_argument(buildLog(std::format(
          "The minimum speed rateo ({}) must be between 0 and 1", minSpeedRateo)));
    }
    m_minSpeedRateo = minSpeedRateo;
  }

  template <typename Delay>
    requires(is_numeric_v<Delay>)
  void Dynamics<Delay>::setErrorProbability(double errorProbability) {
    if (errorProbability < 0. || errorProbability > 1.) {
      throw std::invalid_argument(buildLog(std::format(
          "The error probability ({}) must be between 0 and 1", errorProbability)));
    }
    m_errorProbability = errorProbability;
  }

  template <typename Delay>
    requires(is_numeric_v<Delay>)
  void Dynamics<Delay>::setMaxFlowPercentage(double maxFlowPercentage) {
    if (maxFlowPercentage < 0. || maxFlowPercentage > 1.) {
      throw std::invalid_argument(
          buildLog(std::format("The maximum flow percentage ({}) must be between 0 and 1",
                               maxFlowPercentage)));
    }
    m_maxFlowPercentage = maxFlowPercentage;
  }

  template <typename Delay>
    requires(is_numeric_v<Delay>)
  void Dynamics<Delay>::setDestinationNodes(const std::span<Id>& destinationNodes,
                                            bool updatePaths) {
    for (const auto& nodeId : destinationNodes) {
      if (!m_graph.nodeSet().contains(nodeId)) {
        throw std::invalid_argument(
            buildLog(std::format("Node with id {} not found", nodeId)));
      }
      this->addItinerary(Itinerary{nodeId, nodeId});
    }
    if (updatePaths) {
      this->updatePaths();
    }
  }

  template <typename Delay>
    requires(is_numeric_v<Delay>)
  void Dynamics<Delay>::updatePaths() {
    std::vector<std::thread> threads;
    threads.reserve(m_itineraries.size());
    std::exception_ptr pThreadException;
    for (const auto& [itineraryId, itinerary] : m_itineraries) {
      threads.emplace_back(std::thread([this, &itinerary, &pThreadException] {
        try {
          this->m_updatePath(itinerary);
        } catch (...) {
          if (!pThreadException)
            pThreadException = std::current_exception();
        }
      }));
    }
    for (auto& thread : threads) {
      thread.join();
    }
    // Throw the exception launched first
    if (pThreadException)
      std::rethrow_exception(pThreadException);
  }

  template <typename Delay>
    requires(is_numeric_v<Delay>)
  void Dynamics<Delay>::evolve(bool reinsert_agents) {
    // move the first agent of each street queue, if possible, putting it in the next node
    this->m_evolveStreets(reinsert_agents);
    // move all the agents from each node, if possible
    this->m_evolveNodes();
    // cycle over agents and update their times
    this->m_evolveAgents();
    // increment time simulation
    ++this->m_time;
  }

  template <typename Delay>
    requires(is_numeric_v<Delay>)
  void Dynamics<Delay>::optimizeTrafficLights(Delay nCycles,
                                              double threshold,
                                              double densityTolerance) {
    if (threshold < 0 || threshold > 1) {
      throw std::invalid_argument(
          buildLog(std::format("The threshold parameter is a percentage and must be "
                               "bounded between 0-1. Inserted value: {}",
                               threshold)));
    }
    if (densityTolerance < 0 || densityTolerance > 1) {
      throw std::invalid_argument(buildLog(
          std::format("The densityTolerance parameter is a percentage and must be "
                      "bounded between 0-1. Inserted value: {}",
                      densityTolerance)));
    }
    const auto meanDensityGlob = streetMeanDensity().mean;  // Measurement<double>
    for (const auto& [nodeId, node] : m_graph.nodeSet()) {
      if (!node->isTrafficLight()) {
        continue;
      }
      auto& tl = dynamic_cast<TrafficLight<Delay>&>(*node);
      if (!tl.delay().has_value()) {
        continue;
      }
      auto [greenTime, redTime] = tl.delay().value();
      const auto cycleTime = greenTime + redTime;
      const auto& streetPriorities = tl.streetPriorities();
      Size greenSum{0}, greenQueue{0};
      Size redSum{0}, redQueue{0};
      for (const auto& [streetId, _] : m_graph.adjMatrix().getCol(nodeId, true)) {
        if (streetPriorities.contains(streetId)) {
          greenSum += m_streetTails[streetId];
          greenQueue += m_graph.streetSet()[streetId]->queue().size();
        } else {
          redSum += m_streetTails[streetId];
          redQueue += m_graph.streetSet()[streetId]->queue().size();
        }
      }
      const Delay delta =
          std::floor(std::fabs(static_cast<int>(greenQueue - redQueue)) / nCycles);
      if (delta == 0) {
        continue;
      }
      const Size smallest = std::min(greenSum, redSum);
      if (std::abs(static_cast<int>(greenSum - redSum)) < threshold * smallest) {
        tl.setDelay(std::floor(cycleTime / 2));
        continue;
      }
      // If the difference is not less than the threshold
      //    - Check that the incoming streets have a density less than the mean one (eventually + tolerance): I want to avoid being into the cluster, better to be out or on the border
      //    - If the previous check fails, do nothing
      double meanDensity_streets{0.};
      {
        // Store the ids of outgoing streets
        const auto& row{m_graph.adjMatrix().getRow(nodeId, true)};
        for (const auto& [streetId, _] : row) {
          meanDensity_streets += m_graph.streetSet()[streetId]->density();
        }
        // Take the mean density of the outgoing streets
        const auto nStreets = row.size();
        if (nStreets > 1) {
          meanDensity_streets /= nStreets;
        }
      }
      //std::cout << '\t' << " -> Mean network density: " << std::setprecision(7) << meanDensityGlob << '\n';
      //std::cout << '\t' << " -> Mean density of 4 outgoing streets: " << std::setprecision(7) << meanDensity_streets << '\n';
      const auto ratio = meanDensityGlob / meanDensity_streets;
      // densityTolerance represents the max border we want to consider
      const auto dyn_thresh = std::tanh(ratio) * densityTolerance;
      //std::cout << '\t' << " -> Parametro ratio: " << std::setprecision(7) << ratio << '\n';
      //std::cout << '\t' << " -> Parametro dyn_thresh: " << std::setprecision(7) << dyn_thresh << '\n';
      if (meanDensityGlob * (1. + dyn_thresh) > meanDensity_streets) {
        //std::cout << '\t' << " -> I'm on the cluster's border" << '\n';
        if (meanDensityGlob > meanDensity_streets) {
          //std::cout << '\t' << " -> LESS than max density" << '\n';
          if (!(redTime > greenTime) && (redSum > greenSum) && (greenTime > delta)) {
            greenTime -= delta;
            redTime += delta;
            tl.setDelay(std::make_pair(greenTime, redTime));
          } else if (!(greenTime > redTime) && (greenSum > redSum) && (redTime > delta)) {
            greenTime += delta;
            redTime -= delta;
            tl.setDelay(std::make_pair(greenTime, redTime));
          } else {
            //std::cout << '\t' << " -> NOT entered into previous ifs" << '\n';
            tl.setDelay(std::make_pair(greenTime, redTime));
          }
          //std::cout << '\t' << " -> greenTime: " << static_cast<unsigned int>(greenTime) << '\n';
          //std::cout << '\t' << " -> redTime: " << static_cast<unsigned int>(redTime) << '\n';
          //std::cout << '\t' << " -> modTime: " << tl.modTime() << '\n';
        } else {
          //std::cout << '\t' << " -> GREATER than max density" << '\n';
          if (!(redTime > greenTime) && (redSum > greenSum) &&
              (greenTime > ratio * delta)) {
            greenTime -= dyn_thresh * delta;  //
            redTime += delta;
            tl.setDelay(std::make_pair(greenTime, redTime));
          } else if (!(greenTime > redTime) && (greenSum > redSum) &&
                     (redTime > ratio * delta)) {
            greenTime += delta;
            redTime -= dyn_thresh * delta;  //
            tl.setDelay(std::make_pair(greenTime, redTime));
          } else if (!(redTime > greenTime) && (redSum < greenSum) && (redTime > delta)) {
            greenTime += dyn_thresh * delta;  //
            redTime -= delta;
            tl.setDelay(std::make_pair(greenTime, redTime));
          } else if (!(redTime < greenTime) && (redSum > greenSum) &&
                     (greenTime > delta)) {
            greenTime -= delta;
            redTime += dyn_thresh * delta;  //
            tl.setDelay(std::make_pair(greenTime, redTime));
          } else {
            //std::cout << '\t' << " -> NON sono entrato negli if precedenti" << '\n';
            tl.setDelay(std::make_pair(greenTime, redTime));
          }
          //std::cout << '\t' << " -> greenTime: " << static_cast<unsigned int>(greenTime) << '\n';
          //std::cout << '\t' << " -> redTime: " << static_cast<unsigned int>(redTime) << '\n';
          //std::cout << '\t' << " -> modTime: " << tl.modTime() << '\n';
        }
      } else {
        //std::cout << '\t' << " -> I'm INTO the cluster" << '\n';
        //std::cout << '\t' << " -> modTime: " << tl.modTime() << '\n';
      }
    }
    for (auto& [id, element] : m_streetTails) {
      element = 0;
    }
  }

  template <typename Delay>
    requires(is_numeric_v<Delay>)
  void Dynamics<Delay>::addAgent(const Agent<Delay>& agent) {
    if (this->m_agents.size() + 1 > this->m_graph.maxCapacity()) {
      throw std::overflow_error(buildLog(
          std::format("Graph its already holding the max possible number of agents ({})",
                      this->m_graph.maxCapacity())));
    }
    if (this->m_agents.contains(agent.id())) {
      throw std::invalid_argument(
          buildLog(std::format("Agent with id {} already exists.", agent.id())));
    }
    this->m_agents.emplace(agent.id(), std::make_unique<Agent<Delay>>(agent));
  }
  template <typename Delay>
    requires(is_numeric_v<Delay>)
  void Dynamics<Delay>::addAgent(std::unique_ptr<Agent<Delay>> agent) {
    if (this->m_agents.size() + 1 > this->m_graph.maxCapacity()) {
      throw std::overflow_error(buildLog(
          std::format("Graph its already holding the max possible number of agents ({})",
                      this->m_graph.maxCapacity())));
    }
    if (this->m_agents.contains(agent->id())) {
      throw std::invalid_argument(
          buildLog(std::format("Agent with id {} already exists.", agent->id())));
    }
    this->m_agents.emplace(agent->id(), std::move(agent));
  }
  template <typename Delay>
    requires(is_numeric_v<Delay>)
  void Dynamics<Delay>::addAgent(Id srcNodeId, Id itineraryId) {
    if (this->m_agents.size() + 1 > this->m_graph.maxCapacity()) {
      throw std::overflow_error(buildLog(
          std::format("Graph its already holding the max possible number of agents ({})",
                      this->m_graph.maxCapacity())));
    }
    if (!(srcNodeId < this->m_graph.nodeSet().size())) {
      throw std::invalid_argument(
          buildLog(std::format("Node with id {} not found", srcNodeId)));
    }
    if (!(this->m_itineraries.contains(itineraryId))) {
      throw std::invalid_argument(
          buildLog(std::format("Itinerary with id {} not found", itineraryId)));
    }
    Size agentId{0};
    if (!this->m_agents.empty()) {
      agentId = this->m_agents.rbegin()->first + 1;
    }
    this->addAgent(Agent<Delay>{agentId, itineraryId, srcNodeId});
  }
  template <typename Delay>
    requires(is_numeric_v<Delay>)
  void Dynamics<Delay>::addAgents(Id itineraryId,
                                  Size nAgents,
                                  std::optional<Id> srcNodeId) {
    if (this->m_agents.size() + nAgents > this->m_graph.maxCapacity()) {
      throw std::overflow_error(buildLog(
          std::format("Graph its already holding the max possible number of agents ({})",
                      this->m_graph.maxCapacity())));
    }
    auto itineraryIt{m_itineraries.find(itineraryId)};
    if (itineraryIt == m_itineraries.end()) {
      throw std::invalid_argument(
          buildLog(std::format("Itinerary with id {} not found", itineraryId)));
    }
    Size agentId{0};
    if (!this->m_agents.empty()) {
      agentId = this->m_agents.rbegin()->first + 1;
    }
    for (Size i{0}; i < nAgents; ++i, ++agentId) {
      this->addAgent(Agent<Delay>{agentId, itineraryId});
      if (srcNodeId.has_value()) {
        this->m_agents[agentId]->setSourceNodeId(srcNodeId.value());
      }
    }
  }

  template <typename Delay>
    requires(is_numeric_v<Delay>)
  template <typename... Tn>
    requires(is_agent_v<Tn> && ...)
  void Dynamics<Delay>::addAgents(Tn... agents) {}

  template <typename Delay>
    requires(is_numeric_v<Delay>)
  template <typename T1, typename... Tn>
    requires(is_agent_v<T1> && (is_agent_v<Tn> && ...))
  void Dynamics<Delay>::addAgents(T1 agent, Tn... agents) {
    addAgent(agent);
    addAgents(agents...);
  }

  template <typename Delay>
    requires(is_numeric_v<Delay>)
  void Dynamics<Delay>::addAgents(std::span<Agent<Delay>> agents) {
    if (this->m_agents.size() + agents.size() > this->m_graph.maxCapacity()) {
      throw std::overflow_error(buildLog(
          std::format("Graph its already holding the max possible number of agents ({})",
                      this->m_graph.maxCapacity())));
    }
    std::ranges::for_each(agents, [this](const auto& agent) -> void {
      this->m_agents.push_back(std::make_unique(agent));
    });
  }

  template <typename Delay>
    requires(is_numeric_v<Delay>)
  void Dynamics<Delay>::addAgentsUniformly(Size nAgents, std::optional<Id> itineraryId) {
    if (this->m_agents.size() + nAgents > this->m_graph.maxCapacity()) {
      throw std::overflow_error(buildLog(
          std::format("Graph its already holding the max possible number of agents ({})",
                      this->m_graph.maxCapacity())));
    }
    if (this->m_itineraries.empty()) {
      // TODO: make this possible for random agents
      throw std::invalid_argument(
          buildLog("It is not possible to add random agents without itineraries."));
    }
    const bool randomItinerary{!itineraryId.has_value()};
    std::uniform_int_distribution<Size> itineraryDist{
        0, static_cast<Size>(this->m_itineraries.size() - 1)};
    std::uniform_int_distribution<Size> streetDist{
        0, static_cast<Size>(this->m_graph.streetSet().size() - 1)};
    for (Size i{0}; i < nAgents; ++i) {
      if (randomItinerary) {
        auto itineraryIt{this->m_itineraries.begin()};
        std::advance(itineraryIt, itineraryDist(this->m_generator));
        itineraryId = itineraryIt->first;
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
      } while (this->m_graph.streetSet()[streetId]->isFull());
      const auto& street{this->m_graph.streetSet()[streetId]};
      Agent<Delay> agent{agentId, itineraryId.value(), street->nodePair().first};
      agent.setStreetId(streetId);
      this->addAgent(agent);
      this->setAgentSpeed(agentId);
      this->m_agents[agentId]->incrementDelay(
          std::ceil(street->length() / this->m_agents[agentId]->speed()));
      street->addAgent(agentId);
      ++agentId;
    }
  }

  template <typename Delay>
    requires(is_numeric_v<Delay>)
  template <typename TContainer>
    requires(std::is_same_v<TContainer, std::unordered_map<Id, double>> ||
             std::is_same_v<TContainer, std::map<Id, double>>)
  void Dynamics<Delay>::addAgentsRandomly(Size nAgents,
                                          const TContainer& src_weights,
                                          const TContainer& dst_weights) {
    // Check if the weights are normalized
    if (std::abs(std::accumulate(src_weights.begin(),
                                 src_weights.end(),
                                 0.,
                                 [](double sum, const std::pair<Id, double>& p) {
                                   return sum + p.second;
                                 }) -
                 1.) > std::numeric_limits<double>::epsilon()) {
      throw std::invalid_argument(
          buildLog("The source weights are not normalized (sum is {})."));
    }
    if (std::abs(std::accumulate(dst_weights.begin(),
                                 dst_weights.end(),
                                 0.,
                                 [](double sum, const std::pair<Id, double>& p) {
                                   return sum + p.second;
                                 }) -
                 1.) > std::numeric_limits<double>::epsilon()) {
      throw std::invalid_argument(
          buildLog("The destination weights are not normalized (sum is {})."));
    }
    while (nAgents > 0) {
      Id srcId{0}, dstId{0};
      double dRand{this->m_uniformDist(this->m_generator)}, sum{0.};
      for (const auto& [id, weight] : src_weights) {
        sum += weight;
        if (dRand < sum) {
          srcId = id;
          break;
        }
      }
      dRand = this->m_uniformDist(this->m_generator);
      sum = 0.;
      for (const auto& [id, weight] : dst_weights) {
        sum += weight;
        if (dRand < sum) {
          dstId = id;
          break;
        }
      }
      // find the itinerary with the given destination as destination
      auto itineraryIt{std::find_if(
          m_itineraries.begin(), m_itineraries.end(), [dstId](const auto& itinerary) {
            return itinerary.second->destination() == dstId;
          })};
      if (itineraryIt == m_itineraries.end()) {
        throw std::invalid_argument(
            buildLog(std::format("Itinerary with destination {} not found.", dstId)));
      }
      this->addAgent(srcId, itineraryIt->first);
      --nAgents;
    }
  }

  template <typename Delay>
    requires(is_numeric_v<Delay>)
  void Dynamics<Delay>::removeAgent(Size agentId) {
    auto agentIt{m_agents.find(agentId)};
    if (agentIt == m_agents.end()) {
      throw std::invalid_argument(
          buildLog(std::format("Agent with id {} not found.", agentId)));
    }
    m_agents.erase(agentId);
  }

  template <typename Delay>
    requires(is_numeric_v<Delay>)
  template <typename T1, typename... Tn>
    requires(std::is_convertible_v<T1, Size> && (std::is_convertible_v<Tn, Size> && ...))
  void Dynamics<Delay>::removeAgents(T1 id, Tn... ids) {
    removeAgent(id);
    removeAgents(ids...);
  }

  template <typename Delay>
    requires(is_numeric_v<Delay>)
  void Dynamics<Delay>::addItinerary(const Itinerary& itinerary) {
    m_itineraries.emplace(itinerary.id(), std::make_unique<Itinerary>(itinerary));
  }

  template <typename Delay>
    requires(is_numeric_v<Delay>)
  void Dynamics<Delay>::addItinerary(std::unique_ptr<Itinerary> itinerary) {
    m_itineraries.emplace(itinerary->id(), std::move(itinerary));
  }

  template <typename Delay>
    requires(is_numeric_v<Delay>)
  template <typename... Tn>
    requires(is_itinerary_v<Tn> && ...)
  void Dynamics<Delay>::addItineraries(Tn... itineraries) {
    (this->addItinerary(itineraries), ...);
  }

  template <typename Delay>
    requires(is_numeric_v<Delay>)
  void Dynamics<Delay>::addItineraries(std::span<Itinerary> itineraries) {
    std::ranges::for_each(itineraries, [this](const auto& itinerary) -> void {
      this->m_itineraries.insert(std::make_unique<Itinerary>(itinerary));
    });
  }

  template <typename Delay>
    requires(is_numeric_v<Delay>)
  void Dynamics<Delay>::resetTime() {
    m_time = 0;
  }

  // template <typename Delay>
  //   requires(
  //            is_numeric_v<Delay>)
  // template <typename F, typename... Tn>
  //   requires(std::is_invocable_v<F, Tn...>)
  // void Dynamics<Delay>::evolve(F f, Tn... args) {
  //   f(args...);
  // }

  template <typename Delay>
    requires(is_numeric_v<Delay>)
  Measurement<double> Dynamics<Delay>::agentMeanSpeed() const {
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

  template <typename Delay>
    requires(is_numeric_v<Delay>)
  Measurement<double> Dynamics<Delay>::streetMeanDensity() const {
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

  template <typename Delay>
    requires(is_numeric_v<Delay>)
  Measurement<double> Dynamics<Delay>::streetMeanFlow() const {
    std::vector<double> flows;
    flows.reserve(m_graph.streetSet().size());
    for (const auto& [streetId, street] : m_graph.streetSet()) {
      flows.push_back(street->density() * this->streetMeanSpeed(streetId));
    }
    return Measurement(flows);
  }

  template <typename Delay>
    requires(is_numeric_v<Delay>)
  Measurement<double> Dynamics<Delay>::streetMeanFlow(double threshold,
                                                      bool above) const {
    std::vector<double> flows;
    flows.reserve(m_graph.streetSet().size());
    for (const auto& [streetId, street] : m_graph.streetSet()) {
      if (above and (street->normDensity() > threshold)) {
        flows.push_back(street->density() * this->streetMeanSpeed(streetId));
      } else if (!above and (street->normDensity() < threshold)) {
        flows.push_back(street->density() * this->streetMeanSpeed(streetId));
      }
    }
    return Measurement(flows);
  }

  template <typename Delay>
    requires(is_numeric_v<Delay>)
  Measurement<double> Dynamics<Delay>::meanSpireInputFlow(bool resetValue) {
    auto deltaTime{m_time - m_previousSpireTime};
    if (deltaTime == 0) {
      return Measurement(0., 0.);
    }
    m_previousSpireTime = m_time;
    std::vector<double> flows;
    flows.reserve(m_graph.streetSet().size());
    for (const auto& [streetId, street] : m_graph.streetSet()) {
      if (street->isSpire()) {
        auto& spire = dynamic_cast<SpireStreet&>(*street);
        flows.push_back(static_cast<double>(spire.inputCounts(resetValue)) / deltaTime);
      }
    }
    return Measurement(flows);
  }

  template <typename Delay>
    requires(is_numeric_v<Delay>)
  Measurement<double> Dynamics<Delay>::meanSpireOutputFlow(bool resetValue) {
    auto deltaTime{m_time - m_previousSpireTime};
    if (deltaTime == 0) {
      return Measurement(0., 0.);
    }
    m_previousSpireTime = m_time;
    std::vector<double> flows;
    flows.reserve(m_graph.streetSet().size());
    for (const auto& [streetId, street] : m_graph.streetSet()) {
      if (street->isSpire()) {
        auto& spire = dynamic_cast<SpireStreet&>(*street);
        flows.push_back(static_cast<double>(spire.outputCounts(resetValue)) / deltaTime);
      }
    }
    return Measurement(flows);
  }

  template <typename Delay>
    requires(is_numeric_v<Delay>)
  Measurement<double> Dynamics<Delay>::meanTravelTime(bool clearData) {
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

  template <typename Delay>
    requires(is_numeric_v<Delay>)
  std::unordered_map<Id, std::array<double, 4>> Dynamics<Delay>::turnProbabilities(
      bool reset) {
    std::unordered_map<Id, std::array<double, 4>> res;
    for (auto& [streetId, counts] : m_turnCounts) {
      std::array<double, 4> probabilities{0., 0., 0., 0.};
      const auto sum{std::accumulate(counts.cbegin(), counts.cend(), 0.)};
      if (sum != 0) {
        for (auto i{0}; i < counts.size(); ++i) {
          probabilities[i] = counts[i] / sum;
        }
      }
      res.emplace(streetId, probabilities);
    }
    if (reset) {
      for (auto& [streetId, counts] : m_turnCounts) {
        std::fill(counts.begin(), counts.end(), 0);
      }
    }
    return res;
  }

  template <typename Delay>
    requires(std::unsigned_integral<Delay>)
  class FirstOrderDynamics : public Dynamics<Delay> {
    double m_speedFluctuationSTD;

  public:
    /// @brief Construct a new First Order Dynamics object
    /// @param graph, The graph representing the network
    FirstOrderDynamics(Graph& graph)
        : Dynamics<Delay>(graph), m_speedFluctuationSTD{0.} {};
    /// @brief Set the speed of an agent
    /// @param agentId The id of the agent
    /// @throw std::invalid_argument, If the agent is not found
    void setAgentSpeed(Size agentId) override;
    /// @brief Set the standard deviation of the speed fluctuation
    /// @param speedFluctuationSTD The standard deviation of the speed fluctuation
    /// @throw std::invalid_argument, If the standard deviation is negative
    void setSpeedFluctuationSTD(double speedFluctuationSTD);
    /// @brief Get the mean speed of a street in \f$m/s\f$
    /// @return double The mean speed of the street or street->maxSpeed() if the street is empty
    /// @details The mean speed of a street is given by the formula:
    /// \f$ v_{\text{mean}} = v_{\text{max}} \left(1 - \frac{\alpha}{2} \left( n - 1\right)  \right) \f$
    /// where \f$ v_{\text{max}} \f$ is the maximum speed of the street, \f$ \alpha \f$ is the minimum speed rateo divided by the capacity
    /// and \f$ n \f$ is the number of agents in the street
    double streetMeanSpeed(Id streetId) const override;
    /// @brief Get the mean speed of the streets in \f$m/s\f$
    /// @return Measurement The mean speed of the agents and the standard deviation
    Measurement<double> streetMeanSpeed() const override;
    /// @brief Get the mean speed of the streets with density above or below a threshold in \f$m/s\f$
    /// @param threshold The density threshold to consider
    /// @param above If true, the function returns the mean speed of the streets with a density above the threshold, otherwise below
    /// @return Measurement The mean speed of the agents and the standard deviation
    Measurement<double> streetMeanSpeed(double threshold, bool above) const override;
  };

  template <typename Delay>
    requires(std::unsigned_integral<Delay>)
  void FirstOrderDynamics<Delay>::setAgentSpeed(Size agentId) {
    const auto& agent{this->m_agents[agentId]};
    const auto& street{this->m_graph.streetSet()[agent->streetId().value()]};
    double speed{street->maxSpeed() *
                 (1. - this->m_minSpeedRateo * street->normDensity())};
    if (this->m_speedFluctuationSTD > 0.) {
      std::normal_distribution<double> speedDist{speed,
                                                 speed * this->m_speedFluctuationSTD};
      speed = speedDist(this->m_generator);
    }
    speed < 0. ? agent->setSpeed(street->maxSpeed() * (1. - this->m_minSpeedRateo))
               : agent->setSpeed(speed);
  }

  template <typename Delay>
    requires(std::unsigned_integral<Delay>)
  void FirstOrderDynamics<Delay>::setSpeedFluctuationSTD(double speedFluctuationSTD) {
    if (speedFluctuationSTD < 0.) {
      throw std::invalid_argument(
          buildLog("The speed fluctuation standard deviation must be positive."));
    }
    m_speedFluctuationSTD = speedFluctuationSTD;
  }

  template <typename Delay>
    requires(std::unsigned_integral<Delay>)
  double FirstOrderDynamics<Delay>::streetMeanSpeed(Id streetId) const {
    const auto& street{this->m_graph.streetSet().at(streetId)};
    if (street->nAgents() == 0) {
      return street->maxSpeed();
    }
    double meanSpeed{0.};
    Size n{0};
    if (street->queue().size() == 0) {
      n = static_cast<Size>(street->waitingAgents().size());
      double alpha{this->m_minSpeedRateo / street->capacity()};
      meanSpeed = street->maxSpeed() * n * (1. - 0.5 * alpha * (n - 1.));
    } else {
      for (const auto& agentId : street->waitingAgents()) {
        meanSpeed += this->m_agents.at(agentId)->speed();
        ++n;
      }
      for (const auto& agentId : street->queue()) {
        meanSpeed += this->m_agents.at(agentId)->speed();
        ++n;
      }
    }
    const auto& node = this->m_graph.nodeSet().at(street->nodePair().second);
    if (node->isIntersection()) {
      auto& intersection = dynamic_cast<Node&>(*node);
      for (const auto& [angle, agentId] : intersection.agents()) {
        const auto& agent{this->m_agents.at(agentId)};
        if (agent->streetId().has_value() && agent->streetId().value() == streetId) {
          meanSpeed += agent->speed();
          ++n;
        }
      }
    } else if (node->isRoundabout()) {
      auto& roundabout = dynamic_cast<Roundabout&>(*node);
      for (const auto& agentId : roundabout.agents()) {
        const auto& agent{this->m_agents.at(agentId)};
        if (agent->streetId().has_value() && agent->streetId().value() == streetId) {
          meanSpeed += agent->speed();
          ++n;
        }
      }
    }
    return meanSpeed / n;
  }

  template <typename Delay>
    requires(std::unsigned_integral<Delay>)
  Measurement<double> FirstOrderDynamics<Delay>::streetMeanSpeed() const {
    if (this->m_agents.size() == 0) {
      return Measurement(0., 0.);
    }
    std::vector<double> speeds;
    speeds.reserve(this->m_graph.streetSet().size());
    for (const auto& [streetId, street] : this->m_graph.streetSet()) {
      speeds.push_back(this->streetMeanSpeed(streetId));
    }
    return Measurement(speeds);
  }
  template <typename Delay>
    requires(std::unsigned_integral<Delay>)
  Measurement<double> FirstOrderDynamics<Delay>::streetMeanSpeed(double threshold,
                                                                 bool above) const {
    if (this->m_agents.size() == 0) {
      return Measurement(0., 0.);
    }
    std::vector<double> speeds;
    speeds.reserve(this->m_graph.streetSet().size());
    for (const auto& [streetId, street] : this->m_graph.streetSet()) {
      if (above) {
        if (street->normDensity() > threshold) {
          speeds.push_back(this->streetMeanSpeed(streetId));
        }
      } else {
        if (street->normDensity() < threshold) {
          speeds.push_back(this->streetMeanSpeed(streetId));
        }
      }
    }
    return Measurement(speeds);
  }

  /* class SecondOrderDynamics : public Dynamics<double> { */
  /* public: */
  /*   void setAgentSpeed(Size agentId); */
  /*   void setSpeed(); */
  /* }; */

  /* void SecondOrderDynamics::setAgentSpeed(Size agentId) {} */

  /* void SecondOrderDynamics::setSpeed() {} */

};  // namespace dsm
