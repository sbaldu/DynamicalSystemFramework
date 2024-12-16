/// @file       /src/dsm/headers/Dynamics.hpp
/// @brief      Defines the Dynamics class.
///
/// @details    This file contains the definition of the Dynamics class.
///             The Dynamics class represents the dynamics of the network. It is templated by the type
///             of the graph's id and the type of the graph's capacity.
///             The graph's id and capacity must be unsigned integral types.

#pragma once

#include <algorithm>
#include <cassert>
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
#include <exception>

#include "DijkstraWeights.hpp"
#include "Itinerary.hpp"
#include "Graph.hpp"
#include "SparseMatrix.hpp"
#include "../utility/TypeTraits/is_agent.hpp"
#include "../utility/TypeTraits/is_itinerary.hpp"
#include "../utility/Logger.hpp"
#include "../utility/Typedef.hpp"

namespace dsm {
  /// @brief The Measurement struct represents the mean of a quantity and its standard deviation
  /// @tparam T The type of the quantity
  /// @param mean The mean
  /// @param std The standard deviation of the sample
  template <typename T>
  struct Measurement {
    T mean;
    T std;

    Measurement(T mean, T std) : mean{mean}, std{std} {}
    Measurement(std::span<T> data) {
      auto x_mean = static_cast<T>(0), x2_mean = static_cast<T>(0);
      if (data.empty()) {
        mean = static_cast<T>(0);
        std = static_cast<T>(0);
        return;
      }

      std::for_each(data.begin(), data.end(), [&x_mean, &x2_mean](auto value) -> void {
        x_mean += value;
        x2_mean += value * value;
      });
      mean = x_mean / data.size();
      std = std::sqrt(x2_mean / data.size() - mean * mean);
    }
  };

  /// @brief The Dynamics class represents the dynamics of the network.
  /// @tparam Id, The type of the graph's id. It must be an unsigned integral type.
  /// @tparam Size, The type of the graph's capacity. It must be an unsigned integral type.
  template <typename agent_t>
  class Dynamics {
  protected:
    std::unordered_map<Id, std::unique_ptr<Itinerary>> m_itineraries;
    std::map<Id, std::unique_ptr<agent_t>> m_agents;
    Graph m_graph;
    Time m_time, m_previousSpireTime;
    std::mt19937_64 m_generator;

    virtual void m_evolveStreet(const std::unique_ptr<Street>& pStreet,
                                bool reinsert_agents) = 0;
    virtual bool m_evolveNode(const std::unique_ptr<Node>& pNode) = 0;
    virtual void m_evolveAgents() = 0;

    /// @brief Update the path of a single itinerary using Dijsktra's algorithm
    /// @param pItinerary An std::unique_prt to the itinerary
    void m_updatePath(const std::unique_ptr<Itinerary>& pItinerary) {
      Size const dimension = m_graph.adjMatrix().getRowDim();
      auto const destinationID = pItinerary->destination();
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
                  m_graph.streetSet()[nodeId * dimension + nextNodeId]->length()) {
            path.insert(nodeId, nextNodeId, true);
            continue;
          }
          result = m_graph.shortestPath(nextNodeId, destinationID);

          if (result.has_value()) {
            // if the shortest path exists, save the distance
            if (minDistance ==
                result.value().distance() +
                    m_graph.streetSet()[nodeId * dimension + nextNodeId]->length()) {
              path.insert(nodeId, nextNodeId, true);
            }
          } else if ((nextNodeId != destinationID)) {
            std::cerr << std::format(
                             "\033[38;2;130;30;180mWARNING: No path found from node {} "
                             "to node {}\033[0m",
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
    Dynamics(Graph& graph, std::optional<unsigned int> seed);

    virtual void setAgentSpeed(Size agentId) = 0;
    virtual void evolve(bool reinsert_agents = false) = 0;

    /// @brief Update the paths of the itineraries based on the actual travel times
    virtual void updatePaths();

    /// @brief Set the dynamics destination nodes auto-generating itineraries
    /// @param destinationNodes The destination nodes
    /// @param updatePaths If true, the paths are updated
    /// @throws std::invalid_argument Ifone or more destination nodes do not exist
    void setDestinationNodes(const std::span<Id>& destinationNodes,
                             bool updatePaths = true);

    /// @brief Add an agent to the simulation
    /// @param agent The agent
    void addAgent(const agent_t& agent);
    /// @brief Add an agent to the simulation
    /// @param agent std::unique_ptr to the agent
    void addAgent(std::unique_ptr<agent_t> agent);
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
    void addAgents(std::span<agent_t> agents);
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

    void addRandomAgents(Size nAgents, std::optional<Id> srcNodeId = std::nullopt);

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
    const std::map<Id, std::unique_ptr<agent_t>>& agents() const { return m_agents; }
    /// @brief Get the time
    /// @return Time The time
    Time time() const { return m_time; }

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
    Measurement<double> streetMeanDensity(bool normalized = false) const;
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
  };

  template <typename agent_t>
  Dynamics<agent_t>::Dynamics(Graph& graph, std::optional<unsigned int> seed)
      : m_graph{std::move(graph)},
        m_time{0},
        m_previousSpireTime{0},
        m_generator{std::random_device{}()} {
    if (seed.has_value()) {
      m_generator.seed(seed.value());
    }
  }

  template <typename agent_t>
  void Dynamics<agent_t>::updatePaths() {
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

  template <typename agent_t>
  void Dynamics<agent_t>::setDestinationNodes(const std::span<Id>& destinationNodes,
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

  template <typename agent_t>
  void Dynamics<agent_t>::addAgent(const agent_t& agent) {
    if (m_agents.size() + 1 > m_graph.maxCapacity()) {
      throw std::overflow_error(buildLog(
          std::format("Graph is already holding the max possible number of agents ({})",
                      m_graph.maxCapacity())));
    }
    if (m_agents.contains(agent.id())) {
      throw std::invalid_argument(
          buildLog(std::format("Agent with id {} already exists.", agent.id())));
    }
    m_agents.emplace(agent.id(), std::make_unique<agent_t>(agent));
  }
  template <typename agent_t>
  void Dynamics<agent_t>::addAgent(std::unique_ptr<agent_t> agent) {
    if (m_agents.size() + 1 > m_graph.maxCapacity()) {
      throw std::overflow_error(buildLog(
          std::format("Graph is already holding the max possible number of agents ({})",
                      m_graph.maxCapacity())));
    }
    if (m_agents.contains(agent->id())) {
      throw std::invalid_argument(
          buildLog(std::format("Agent with id {} already exists.", agent->id())));
    }
    m_agents.emplace(agent->id(), std::move(agent));
  }
  template <typename agent_t>
  void Dynamics<agent_t>::addAgent(Id srcNodeId, Id itineraryId) {
    if (m_agents.size() + 1 > m_graph.maxCapacity()) {
      throw std::overflow_error(buildLog(
          std::format("Graph is already holding the max possible number of agents ({})",
                      m_graph.maxCapacity())));
    }
    if (!(srcNodeId < m_graph.nodeSet().size())) {
      throw std::invalid_argument(
          buildLog(std::format("Node with id {} not found", srcNodeId)));
    }
    if (!(m_itineraries.contains(itineraryId))) {
      throw std::invalid_argument(
          buildLog(std::format("Itinerary with id {} not found", itineraryId)));
    }
    Size agentId{0};
    if (!m_agents.empty()) {
      agentId = m_agents.rbegin()->first + 1;
    }
    this->addAgent(agent_t{agentId, itineraryId, srcNodeId});
  }
  template <typename agent_t>
  void Dynamics<agent_t>::addAgents(Id itineraryId,
                                    Size nAgents,
                                    std::optional<Id> srcNodeId) {
    if (m_agents.size() + nAgents > m_graph.maxCapacity()) {
      throw std::overflow_error(buildLog(
          std::format("Graph is already holding the max possible number of agents ({})",
                      m_graph.maxCapacity())));
    }
    auto itineraryIt{m_itineraries.find(itineraryId)};
    if (itineraryIt == m_itineraries.end()) {
      throw std::invalid_argument(
          buildLog(std::format("Itinerary with id {} not found", itineraryId)));
    }
    Size agentId{0};
    if (!m_agents.empty()) {
      agentId = m_agents.rbegin()->first + 1;
    }
    for (Size i{0}; i < nAgents; ++i, ++agentId) {
      this->addAgent(agent_t{agentId, itineraryId, srcNodeId});
    }
  }

  template <typename agent_t>
  template <typename... Tn>
    requires(is_agent_v<Tn> && ...)
  void Dynamics<agent_t>::addAgents(Tn... agents) {}

  template <typename agent_t>
  template <typename T1, typename... Tn>
    requires(is_agent_v<T1> && (is_agent_v<Tn> && ...))
  void Dynamics<agent_t>::addAgents(T1 agent, Tn... agents) {
    addAgent(agent);
    addAgents(agents...);
  }

  template <typename agent_t>
  void Dynamics<agent_t>::addAgents(std::span<agent_t> agents) {
    if (this->m_agents.size() + agents.size() > this->m_graph.maxCapacity()) {
      throw std::overflow_error(buildLog(
          std::format("Graph is already holding the max possible number of agents ({})",
                      this->m_graph.maxCapacity())));
    }
    std::ranges::for_each(agents, [this](const auto& agent) -> void {
      this->m_agents.push_back(std::make_unique(agent));
    });
  }

  template <typename agent_t>
  void Dynamics<agent_t>::addAgentsUniformly(Size nAgents,
                                             std::optional<Id> itineraryId) {
    if (m_agents.size() + nAgents > m_graph.maxCapacity()) {
      throw std::overflow_error(buildLog(
          std::format("Graph is already holding the max possible number of agents ({})",
                      this->m_graph.maxCapacity())));
    }
    if (m_itineraries.empty()) {
      // TODO: make this possible for random agents
      throw std::invalid_argument(
          buildLog("It is not possible to add random agents without itineraries."));
    }
    const bool randomItinerary{!itineraryId.has_value()};
    std::uniform_int_distribution<Size> itineraryDist{
        0, static_cast<Size>(m_itineraries.size() - 1)};
    std::uniform_int_distribution<Size> streetDist{
        0, static_cast<Size>(m_graph.streetSet().size() - 1)};
    for (Size i{0}; i < nAgents; ++i) {
      if (randomItinerary) {
        auto itineraryIt{m_itineraries.begin()};
        std::advance(itineraryIt, itineraryDist(m_generator));
        itineraryId = itineraryIt->first;
      }
      Id agentId{0};
      if (!m_agents.empty()) {
        agentId = m_agents.rbegin()->first + 1;
      }
      Id streetId{0};
      do {
        // I dunno why this works and the following doesn't
        const auto& streetSet = m_graph.streetSet();
        auto streetIt = streetSet.begin();
        // auto streetIt = this->m_graph->streetSet().begin();
        Size step = streetDist(m_generator);
        std::advance(streetIt, step);
        streetId = streetIt->first;
      } while (m_graph.streetSet()[streetId]->isFull());
      const auto& street{m_graph.streetSet()[streetId]};
      agent_t agent{agentId, itineraryId.value(), street->nodePair().first};
      agent.setStreetId(streetId);
      this->addAgent(agent);
      this->setAgentSpeed(agentId);
      m_agents[agentId]->incrementDelay(
          std::ceil(street->length() / m_agents[agentId]->speed()));
      street->addAgent(agentId);
      ++agentId;
    }
  }

  template <typename agent_t>
  template <typename TContainer>
    requires(std::is_same_v<TContainer, std::unordered_map<Id, double>> ||
             std::is_same_v<TContainer, std::map<Id, double>>)
  void Dynamics<agent_t>::addAgentsRandomly(Size nAgents,
                                            const TContainer& src_weights,
                                            const TContainer& dst_weights) {
    if (src_weights.size() == 1 && dst_weights.size() == 1 &&
        src_weights.begin()->first == dst_weights.begin()->first) {
      throw std::invalid_argument(buildLog(
          std::format("The only source node {} is also the only destination node.",
                      src_weights.begin()->first)));
    }
    auto const srcSum{std::accumulate(
        src_weights.begin(),
        src_weights.end(),
        0.,
        [](double sum, const std::pair<Id, double>& p) {
          if (p.second < 0.) {
            throw std::invalid_argument(buildLog(std::format(
                "Negative weight ({}) for source node {}.", p.second, p.first)));
          }
          return sum + p.second;
        })};
    auto const dstSum{std::accumulate(
        dst_weights.begin(),
        dst_weights.end(),
        0.,
        [](double sum, const std::pair<Id, double>& p) {
          if (p.second < 0.) {
            throw std::invalid_argument(buildLog(std::format(
                "Negative weight ({}) for destination node {}.", p.second, p.first)));
          }
          return sum + p.second;
        })};
    std::uniform_real_distribution<double> srcUniformDist{0., srcSum};
    std::uniform_real_distribution<double> dstUniformDist{0., dstSum};
    while (nAgents > 0) {
      Id srcId{0}, dstId{0};
      if (dst_weights.size() == 1) {
        dstId = dst_weights.begin()->first;
        srcId = dstId;
      }
      double dRand, sum;
      while (srcId == dstId) {
        dRand = srcUniformDist(m_generator);
        sum = 0.;
        for (const auto& [id, weight] : src_weights) {
          srcId = id;
          sum += weight;
          if (dRand < sum) {
            break;
          }
        }
      }
      if (src_weights.size() > 1) {
        dstId = srcId;
      }
      while (dstId == srcId) {
        dRand = dstUniformDist(m_generator);
        sum = 0.;
        for (const auto& [id, weight] : dst_weights) {
          dstId = id;
          sum += weight;
          if (dRand < sum) {
            break;
          }
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

  template <typename agent_t>
  void Dynamics<agent_t>::addRandomAgents(Size nAgents, std::optional<Id> srcNodeId) {
    if (m_agents.size() + nAgents > m_graph.maxCapacity()) {
      throw std::overflow_error(buildLog(
          std::format("Graph is already holding the max possible number of agents ({})",
                      m_graph.maxCapacity())));
    }
    Id agentId{0};
    if (!m_agents.empty()) {
      agentId = m_agents.rbegin()->first + 1;
    }
    for (auto i{0}; i < nAgents; ++i) {
      this->addAgent(agent_t{agentId, srcNodeId});
    }
  }

  template <typename agent_t>
  void Dynamics<agent_t>::removeAgent(Size agentId) {
    m_agents.erase(agentId);
  }

  template <typename agent_t>

  template <typename T1, typename... Tn>
    requires(std::is_convertible_v<T1, Size> && (std::is_convertible_v<Tn, Size> && ...))
  void Dynamics<agent_t>::removeAgents(T1 id, Tn... ids) {
    removeAgent(id);
    removeAgents(ids...);
  }

  template <typename agent_t>
  void Dynamics<agent_t>::addItinerary(const Itinerary& itinerary) {
    m_itineraries.emplace(itinerary.id(), std::make_unique<Itinerary>(itinerary));
  }

  template <typename agent_t>
  void Dynamics<agent_t>::addItinerary(std::unique_ptr<Itinerary> itinerary) {
    m_itineraries.emplace(itinerary->id(), std::move(itinerary));
  }

  template <typename agent_t>
  template <typename... Tn>
    requires(is_itinerary_v<Tn> && ...)
  void Dynamics<agent_t>::addItineraries(Tn... itineraries) {
    (this->addItinerary(itineraries), ...);
  }
  template <typename agent_t>
  void Dynamics<agent_t>::addItineraries(std::span<Itinerary> itineraries) {
    std::ranges::for_each(itineraries, [this](const auto& itinerary) -> void {
      m_itineraries.insert(std::make_unique<Itinerary>(itinerary));
    });
  }

  template <typename agent_t>
  void Dynamics<agent_t>::resetTime() {
    m_time = 0;
  }

  template <typename agent_t>
  Measurement<double> Dynamics<agent_t>::agentMeanSpeed() const {
    std::vector<double> speeds;
    if (!m_agents.empty()) {
      speeds.reserve(m_agents.size());
      for (const auto& [agentId, agent] : m_agents) {
        speeds.push_back(agent->speed());
      }
    }
    return Measurement<double>(speeds);
  }

  template <typename agent_t>
  Measurement<double> Dynamics<agent_t>::streetMeanDensity(bool normalized) const {
    if (m_graph.streetSet().size() == 0) {
      return Measurement(0., 0.);
    }
    std::vector<double> densities;
    densities.reserve(m_graph.streetSet().size());
    for (const auto& [streetId, street] : m_graph.streetSet()) {
      densities.push_back(street->density(normalized));
    }
    return Measurement<double>(densities);
  }

  template <typename agent_t>
  Measurement<double> Dynamics<agent_t>::streetMeanFlow() const {
    std::vector<double> flows;
    flows.reserve(m_graph.streetSet().size());
    for (const auto& [streetId, street] : m_graph.streetSet()) {
      flows.push_back(street->density() * this->streetMeanSpeed(streetId));
    }
    return Measurement<double>(flows);
  }

  template <typename agent_t>
  Measurement<double> Dynamics<agent_t>::streetMeanFlow(double threshold,
                                                        bool above) const {
    std::vector<double> flows;
    flows.reserve(m_graph.streetSet().size());
    for (const auto& [streetId, street] : m_graph.streetSet()) {
      if (above && (street->density(true) > threshold)) {
        flows.push_back(street->density() * this->streetMeanSpeed(streetId));
      } else if (!above && (street->density(true) < threshold)) {
        flows.push_back(street->density() * this->streetMeanSpeed(streetId));
      }
    }
    return Measurement<double>(flows);
  }

  template <typename agent_t>
  Measurement<double> Dynamics<agent_t>::meanSpireInputFlow(bool resetValue) {
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
    return Measurement<double>(flows);
  }

  template <typename agent_t>
  Measurement<double> Dynamics<agent_t>::meanSpireOutputFlow(bool resetValue) {
    auto deltaTime{m_time - m_previousSpireTime};
    if (deltaTime == 0) {
      return Measurement(0., 0.);
    }
    m_previousSpireTime = m_time;
    std::vector<double> flows;
    flows.reserve(m_graph.streetSet().size());
    for (auto const& [streetId, street] : m_graph.streetSet()) {
      if (street->isSpire()) {
        auto& spire = dynamic_cast<SpireStreet&>(*street);
        flows.push_back(static_cast<double>(spire.outputCounts(resetValue)) / deltaTime);
      }
    }
    return Measurement<double>(flows);
  }
};  // namespace dsm
