/// @file       /src/dsm/headers/RoadDynamics.hpp
/// @brief      Defines the RoadDynamics class.
///
/// @details    This file contains the definition of the RoadDynamics class.
///             The RoadDynamics class represents the dynamics of the network. It is templated by the type
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

#include "Dynamics.hpp"
#include "Agent.hpp"
#include "DijkstraWeights.hpp"
#include "Itinerary.hpp"
#include "Graph.hpp"
#include "SparseMatrix.hpp"
#include "../utility/TypeTraits/is_agent.hpp"
#include "../utility/TypeTraits/is_itinerary.hpp"
#include "../utility/Logger.hpp"
#include "../utility/Typedef.hpp"

namespace dsm {
  /// @brief The RoadDynamics class represents the dynamics of the network.
  /// @tparam Id, The type of the graph's id. It must be an unsigned integral type.
  /// @tparam Size, The type of the graph's capacity. It must be an unsigned integral type.
  template <typename delay_t>
    requires(is_numeric_v<delay_t>)
  class RoadDynamics : public Dynamics<Agent<delay_t>> {
  protected:
    Time m_previousOptimizationTime;
    double m_errorProbability;
    double m_passageProbability;
    std::vector<double> m_travelTimes;
    std::unordered_map<Id, Id> m_agentNextStreetId;
    bool m_forcePriorities;
    std::optional<delay_t> m_dataUpdatePeriod;
    std::unordered_map<Id, std::array<unsigned long long, 4>> m_turnCounts;
    std::unordered_map<Id, std::array<long, 4>> m_turnMapping;
    std::unordered_map<Id, Size> m_streetTails;

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
    /// @brief Evolve a street
    /// @param pStreet A std::unique_ptr to the street
    /// @param reinsert_agents If true, the agents are reinserted in the simulation after they reach their destination
    /// @details If possible, removes the first agent of the street's queue, putting it in the destination node.
    /// If the agent is going into the destination node, it is removed from the simulation (and then reinserted if reinsert_agents is true)
    void m_evolveStreet(const std::unique_ptr<Street>& pStreet,
                        bool reinsert_agents) override;
    /// @brief If possible, removes one agent from the node, putting it on the next street.
    /// @param pNode A std::unique_ptr to the node
    /// @return bool True if the agent has been moved, false otherwise
    bool m_evolveNode(const std::unique_ptr<Node>& pNode) override;
    /// @brief Evolve the agents.
    /// @details Puts all new agents on a street, if possible, decrements all delays
    /// and increments all travel times.
    void m_evolveAgents() override;

  public:
    /// @brief Construct a new RoadDynamics object
    /// @param graph The graph representing the network
    /// @param seed The seed for the random number generator
    RoadDynamics(Graph& graph, std::optional<unsigned int> seed);

    /// @brief Set the error probability
    /// @param errorProbability The error probability
    /// @throw std::invalid_argument If the error probability is not between 0 and 1
    void setErrorProbability(double errorProbability);

    void setPassageProbability(double passageProbability);
    /// @brief Set the force priorities flag
    /// @param forcePriorities The flag
    /// @details If true, if an agent cannot move to the next street, the whole node is skipped
    void setForcePriorities(bool forcePriorities) { m_forcePriorities = forcePriorities; }
    /// @brief Set the data update period.
    /// @param dataUpdatePeriod delay_t, The period
    /// @details Some data, i.e. the street queue lengths, are stored only after a fixed amount of time which is represented by this variable.
    void setDataUpdatePeriod(delay_t dataUpdatePeriod) {
      m_dataUpdatePeriod = dataUpdatePeriod;
    }

    /// @brief Evolve the simulation
    /// @details Evolve the simulation by moving the agents and updating the travel times.
    /// In particular:
    /// - Move the first agent of each street queue, if possible, putting it in the next node
    /// - Move the agents from each node, if possible, putting them in the next street and giving them a speed.
    /// If the error probability is not zero, the agents can move to a random street.
    /// If the agent is in the destination node, it is removed from the simulation (and then reinserted if reinsert_agents is true)
    /// - Cycle over agents and update their times
    /// @param reinsert_agents If true, the agents are reinserted in the simulation after they reach their destination
    void evolve(bool reinsert_agents = false) override;
    /// @brief Optimize the traffic lights by changing the green and red times
    /// @param threshold double, The percentage of the mean capacity of the streets used as threshold for the delta between the two tails.
    /// @param densityTolerance double, The algorithm will consider all streets with density up to densityTolerance*meanDensity
    /// @param optimizationType TrafficLightOptimization, The type of optimization. Default is DOUBLE_TAIL
    /// @details The function cycles over the traffic lights and, if the difference between the two tails is greater than
    ///   the threshold multiplied by the mean capacity of the streets, it changes the green and red times of the traffic light, keeping the total cycle time constant.
    ///   The optimizationType parameter can be set to SINGLE_TAIL to use an algorith which looks only at the incoming street tails or to DOUBLE_TAIL to consider both incoming and outgoing street tails.
    void optimizeTrafficLights(double const threshold = 0.,
                               double const densityTolerance = 0.,
                               TrafficLightOptimization optimizationType =
                                   TrafficLightOptimization::DOUBLE_TAIL);
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

  template <typename delay_t>
    requires(is_numeric_v<delay_t>)
  RoadDynamics<delay_t>::RoadDynamics(Graph& graph, std::optional<unsigned int> seed)
      : Dynamics<Agent<delay_t>>(graph, seed),
        m_previousOptimizationTime{0},
        m_errorProbability{0.},
        m_passageProbability{1.},
        m_forcePriorities{false} {
    for (const auto& [streetId, street] : this->m_graph.streetSet()) {
      m_streetTails.emplace(streetId, 0);
      m_turnCounts.emplace(streetId, std::array<unsigned long long, 4>{0, 0, 0, 0});
      // fill turn mapping as [streetId, [left street Id, straight street Id, right street Id, U self street Id]]
      m_turnMapping.emplace(streetId, std::array<long, 4>{-1, -1, -1, -1});
      // Turn mappings
      const auto& srcNodeId = street->nodePair().second;
      for (const auto& [ss, _] : this->m_graph.adjMatrix().getRow(srcNodeId, true)) {
        const auto& delta = street->angle() - this->m_graph.streetSet()[ss]->angle();
        if (std::abs(delta) < std::numbers::pi) {
          if (delta < 0.) {
            m_turnMapping[streetId][dsm::Direction::RIGHT] = ss;
            ;  // right
          } else if (delta > 0.) {
            m_turnMapping[streetId][dsm::Direction::LEFT] = ss;  // left
          } else {
            m_turnMapping[streetId][dsm::Direction::STRAIGHT] = ss;  // straight
          }
        } else {
          m_turnMapping[streetId][dsm::Direction::UTURN] = ss;  // U
        }
      }
    }
  }

  template <typename delay_t>
    requires(is_numeric_v<delay_t>)
  Id RoadDynamics<delay_t>::m_nextStreetId(Id agentId,
                                           Id nodeId,
                                           std::optional<Id> streetId) {
    auto const& pAgent{this->m_agents[agentId]};
    auto possibleMoves = this->m_graph.adjMatrix().getRow(nodeId, true);
    if (!pAgent->isRandom()) {
      std::uniform_real_distribution<double> uniformDist{0., 1.};
      if (this->m_itineraries.size() > 0 &&
          uniformDist(this->m_generator) > m_errorProbability) {
        const auto& it = this->m_itineraries[pAgent->itineraryId()];
        if (it->destination() != nodeId) {
          possibleMoves = it->path().getRow(nodeId, true);
        }
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
    } while (!this->m_graph.nodeSet().at(nodeId)->isRoundabout() and
             streetId.has_value() and
             (this->m_graph.streetSet()[iterator->first]->nodePair().second ==
              this->m_graph.streetSet()[streetId.value()]->nodePair().first) and
             (possibleMoves.size() > 1));
    return iterator->first;
  }

  template <typename delay_t>
    requires(is_numeric_v<delay_t>)
  void RoadDynamics<delay_t>::m_increaseTurnCounts(Id streetId, double delta) {
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

  template <typename delay_t>
    requires(is_numeric_v<delay_t>)
  void RoadDynamics<delay_t>::m_evolveStreet(const std::unique_ptr<Street>& pStreet,
                                             bool reinsert_agents) {
    auto const nLanes = pStreet->nLanes();
    std::uniform_real_distribution<double> uniformDist{0., 1.};
    for (auto queueIndex = 0; queueIndex < nLanes; ++queueIndex) {
      if (pStreet->queue(queueIndex).empty()) {
        continue;
      }
      const auto agentId{pStreet->queue(queueIndex).front()};
      auto const& pAgent{this->m_agents[agentId]};
      if (pAgent->delay() > 0) {
        continue;
      }
      pAgent->setSpeed(0.);
      const auto& destinationNode{this->m_graph.nodeSet()[pStreet->nodePair().second]};
      if (destinationNode->isFull()) {
        continue;
      }
      if (destinationNode->isTrafficLight()) {
        auto& tl = dynamic_cast<TrafficLight&>(*destinationNode);
        auto const direction{pStreet->laneMapping().at(queueIndex)};
        if (!tl.isGreen(pStreet->id(), direction)) {
          continue;
        }
      }
      auto const bCanPass = uniformDist(this->m_generator) < m_passageProbability;
      bool bArrived{false};
      if (!bCanPass) {
        if (pAgent->isRandom()) {
          m_agentNextStreetId.erase(agentId);
          bArrived = true;
        } else {
          continue;
        }
      }
      if (!pAgent->isRandom()) {
        if (destinationNode->id() ==
            this->m_itineraries[pAgent->itineraryId()]->destination()) {
          bArrived = true;
        }
      }
      if (bArrived) {
        pStreet->dequeue(queueIndex);
        m_travelTimes.push_back(pAgent->time());
        if (reinsert_agents) {
          // reset Agent's values
          pAgent->reset();
        } else {
          this->removeAgent(agentId);
        }
        continue;
      }
      auto const& nextStreet{this->m_graph.streetSet()[m_agentNextStreetId[agentId]]};
      if (nextStreet->isFull()) {
        continue;
      }
      pStreet->dequeue(queueIndex);
      assert(destinationNode->id() == nextStreet->nodePair().first);
      if (destinationNode->isIntersection()) {
        auto& intersection = dynamic_cast<Intersection&>(*destinationNode);
        auto const delta{nextStreet->deltaAngle(pStreet->angle())};
        m_increaseTurnCounts(pStreet->id(), delta);
        intersection.addAgent(delta, agentId);
      } else if (destinationNode->isRoundabout()) {
        auto& roundabout = dynamic_cast<Roundabout&>(*destinationNode);
        roundabout.enqueue(agentId);
      }
    }
  }

  template <typename delay_t>
    requires(is_numeric_v<delay_t>)
  bool RoadDynamics<delay_t>::m_evolveNode(const std::unique_ptr<Node>& pNode) {
    if (pNode->isIntersection()) {
      auto& intersection = dynamic_cast<Intersection&>(*pNode);
      if (intersection.agents().empty()) {
        return false;
      }
      for (auto const [angle, agentId] : intersection.agents()) {
        auto const& nextStreet{this->m_graph.streetSet()[m_agentNextStreetId[agentId]]};
        if (nextStreet->isFull()) {
          if (m_forcePriorities) {
            return false;
          }
          continue;
        }
        intersection.removeAgent(agentId);
        this->m_agents[agentId]->setStreetId(nextStreet->id());
        this->setAgentSpeed(agentId);
        this->m_agents[agentId]->incrementDelay(
            std::ceil(nextStreet->length() / this->m_agents[agentId]->speed()));
        nextStreet->addAgent(agentId);
        m_agentNextStreetId.erase(agentId);
        return true;
      }
      return false;
    } else if (pNode->isRoundabout()) {
      auto& roundabout = dynamic_cast<Roundabout&>(*pNode);
      if (roundabout.agents().empty()) {
        return false;
      }
      auto const agentId{roundabout.agents().front()};
      auto const& nextStreet{this->m_graph.streetSet()[m_agentNextStreetId[agentId]]};
      if (!(nextStreet->isFull())) {
        if (this->m_agents[agentId]->streetId().has_value()) {
          const auto streetId = this->m_agents[agentId]->streetId().value();
          auto delta = nextStreet->angle() - this->m_graph.streetSet()[streetId]->angle();
          if (delta > std::numbers::pi) {
            delta -= 2 * std::numbers::pi;
          } else if (delta < -std::numbers::pi) {
            delta += 2 * std::numbers::pi;
          }
          m_increaseTurnCounts(streetId, delta);
        }
        roundabout.dequeue();
        this->m_agents[agentId]->setStreetId(nextStreet->id());
        this->setAgentSpeed(agentId);
        this->m_agents[agentId]->incrementDelay(
            std::ceil(nextStreet->length() / this->m_agents[agentId]->speed()));
        nextStreet->addAgent(agentId);
        m_agentNextStreetId.erase(agentId);
      } else {
        return false;
      }
    }
    return true;
  }

  template <typename delay_t>
    requires(is_numeric_v<delay_t>)
  void RoadDynamics<delay_t>::m_evolveAgents() {
    for (const auto& [agentId, agent] : this->m_agents) {
      if (agent->delay() > 0) {
        const auto& street{this->m_graph.streetSet()[agent->streetId().value()]};
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
          auto const nLanes = street->nLanes();
          bool bArrived{false};
          if (!agent->isRandom()) {
            if (this->m_itineraries[agent->itineraryId()]->destination() ==
                street->nodePair().second) {
              agent->updateItinerary();
            }
            if (this->m_itineraries[agent->itineraryId()]->destination() ==
                street->nodePair().second) {
              bArrived = true;
            }
          }
          if (bArrived) {
            std::uniform_int_distribution<size_t> laneDist{
                0, static_cast<size_t>(nLanes - 1)};
            street->enqueue(agentId, laneDist(this->m_generator));
          } else {
            auto const nextStreetId =
                this->m_nextStreetId(agentId, street->nodePair().second, street->id());
            auto const& pNextStreet{this->m_graph.streetSet()[nextStreetId]};
            m_agentNextStreetId.emplace(agentId, nextStreetId);
            if (nLanes == 1) {
              street->enqueue(agentId, 0);
            } else {
              auto const deltaAngle{pNextStreet->deltaAngle(street->angle())};
              if (std::abs(deltaAngle) < std::numbers::pi) {
                // Lanes are counted as 0 is the far right lane
                if (deltaAngle < 0.) {                   // Right
                  street->enqueue(agentId, 0);           // Always the first lane
                } else if (deltaAngle > 0.) {            // Left
                  street->enqueue(agentId, nLanes - 1);  // Always the last lane
                } else {                                 // Straight
                  std::uniform_int_distribution<size_t> laneDist{
                      0, static_cast<size_t>(nLanes - 2)};
                  street->enqueue(agentId, laneDist(this->m_generator));
                }
              } else {                                 // U turn
                street->enqueue(agentId, nLanes - 1);  // Always the last lane
              }
            }
          }
        }
      } else if (!agent->streetId().has_value() &&
                 !m_agentNextStreetId.contains(agentId)) {
        assert(agent->srcNodeId().has_value());
        const auto& srcNode{this->m_graph.nodeSet()[agent->srcNodeId().value()]};
        if (srcNode->isFull()) {
          continue;
        }
        const auto& nextStreet{
            this->m_graph.streetSet()[this->m_nextStreetId(agentId, srcNode->id())]};
        if (nextStreet->isFull()) {
          continue;
        }
        assert(srcNode->id() == nextStreet->nodePair().first);
        if (srcNode->isIntersection()) {
          auto& intersection = dynamic_cast<Intersection&>(*srcNode);
          intersection.addAgent(0., agentId);
        } else if (srcNode->isRoundabout()) {
          auto& roundabout = dynamic_cast<Roundabout&>(*srcNode);
          roundabout.enqueue(agentId);
        }
        m_agentNextStreetId.emplace(agentId, nextStreet->id());
      } else if (agent->delay() == 0) {
        agent->setSpeed(0.);
      }
      agent->incrementTime();
    }
  }

  template <typename delay_t>
    requires(is_numeric_v<delay_t>)
  void RoadDynamics<delay_t>::setErrorProbability(double errorProbability) {
    if (errorProbability < 0. || errorProbability > 1.) {
      throw std::invalid_argument(buildLog(std::format(
          "The error probability ({}) must be between 0 and 1", errorProbability)));
    }
    m_errorProbability = errorProbability;
  }

  template <typename delay_t>
    requires(is_numeric_v<delay_t>)
  void RoadDynamics<delay_t>::setPassageProbability(double passageProbability) {
    if (passageProbability < 0. || passageProbability > 1.) {
      throw std::invalid_argument(buildLog(std::format(
          "The passage probability ({}) must be between 0 and 1", passageProbability)));
    }
    m_passageProbability = passageProbability;
  }

  template <typename delay_t>
    requires(is_numeric_v<delay_t>)
  void RoadDynamics<delay_t>::evolve(bool reinsert_agents) {
    // move the first agent of each street queue, if possible, putting it in the next node
    bool const bUpdateData =
        m_dataUpdatePeriod.has_value() && this->m_time % m_dataUpdatePeriod.value() == 0;
    for (const auto& [streetId, pStreet] : this->m_graph.streetSet()) {
      if (bUpdateData) {
        m_streetTails[streetId] += pStreet->nExitingAgents();
      }
      for (auto i = 0; i < pStreet->transportCapacity(); ++i) {
        this->m_evolveStreet(pStreet, reinsert_agents);
      }
    }
    // Move transport capacity agents from each node
    for (const auto& [nodeId, pNode] : this->m_graph.nodeSet()) {
      for (auto i = 0; i < pNode->transportCapacity(); ++i) {
        if (!this->m_evolveNode(pNode)) {
          break;
        }
      }
      if (pNode->isTrafficLight()) {
        auto& tl = dynamic_cast<TrafficLight&>(*pNode);
        ++tl;  // Increment the counter
      }
    }
    // cycle over agents and update their times
    this->m_evolveAgents();
    // increment time simulation
    ++this->m_time;
  }

  template <typename delay_t>
    requires(is_numeric_v<delay_t>)
  void RoadDynamics<delay_t>::optimizeTrafficLights(
      double const threshold,
      double const densityTolerance,
      TrafficLightOptimization const optimizationType) {
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
    auto const meanDensityGlob = this->streetMeanDensity().mean;  // Measurement
    for (const auto& [nodeId, pNode] : this->m_graph.nodeSet()) {
      if (!pNode->isTrafficLight()) {
        continue;
      }
      auto& tl = dynamic_cast<TrafficLight&>(*pNode);
      const auto& streetPriorities = tl.streetPriorities();
      Size greenSum{0}, greenQueue{0};
      Size redSum{0}, redQueue{0};
      for (const auto& [streetId, _] : this->m_graph.adjMatrix().getCol(nodeId, true)) {
        if (streetPriorities.contains(streetId)) {
          greenSum += m_streetTails[streetId];
          for (auto const& queue : this->m_graph.streetSet()[streetId]->exitQueues()) {
            greenQueue += queue.size();
          }
        } else {
          redSum += m_streetTails[streetId];
          for (auto const& queue : this->m_graph.streetSet()[streetId]->exitQueues()) {
            redQueue += queue.size();
          }
        }
      }
      const auto nCycles =
          static_cast<double>(this->m_time - m_previousOptimizationTime) /
          m_dataUpdatePeriod.value();
      const delay_t delta =
          std::floor(std::fabs(static_cast<int>(greenQueue - redQueue)) / nCycles);
      // std::cout << std::format("GreenSum: {}, RedSum: {}, Delta: {}, nCycles: {}\n",
      //  greenQueue, redQueue, delta, nCycles);
      if (delta == 0) {
        continue;
      }
      const Size smallest = std::min(greenSum, redSum);
      // std::cout << std::format("GreenSum: {}, RedSum: {}, Smallest: {}\n", greenSum, redSum, smallest);
      // std::cout << std::format("Diff: {}, Threshold * Smallest: {}\n", std::abs(static_cast<int>(greenSum - redSum)), threshold * smallest);
      if (std::abs(static_cast<int>(greenSum - redSum)) < threshold * smallest) {
        tl.resetCycles();
        continue;
      }
      auto const greenTime = tl.maxGreenTime(true);
      auto const redTime = tl.maxGreenTime(false);
      if (optimizationType == TrafficLightOptimization::SINGLE_TAIL) {
        if ((greenSum > redSum) && !(greenTime > redTime) && (redTime > delta) &&
            (greenQueue > redQueue)) {
          tl.increaseGreenTimes(delta);
        } else if ((redSum > greenSum) && !(redTime > greenTime) && (greenTime > delta) &&
                   (redQueue > greenQueue)) {
          tl.decreaseGreenTimes(delta);
        } else {
          tl.resetCycles();
        }
      } else if (optimizationType == TrafficLightOptimization::DOUBLE_TAIL) {
        // If the difference is not less than the threshold
        //    - Check that the incoming streets have a density less than the mean one (eventually + tolerance): I want to avoid being into the cluster, better to be out or on the border
        //    - If the previous check fails, do nothing
        double meanDensity_streets{0.};
        {
          // Store the ids of outgoing streets
          const auto& row{this->m_graph.adjMatrix().getRow(nodeId, true)};
          for (const auto& [streetId, _] : row) {
            meanDensity_streets += this->m_graph.streetSet()[streetId]->density();
          }
          // Take the mean density of the outgoing streets
          const auto nStreets = row.size();
          if (nStreets > 1) {
            meanDensity_streets /= nStreets;
          }
        }
        auto const ratio = meanDensityGlob / meanDensity_streets;
        // densityTolerance represents the max border we want to consider
        auto const dyn_thresh = std::tanh(ratio) * densityTolerance;
        if (meanDensityGlob * (1. + dyn_thresh) > meanDensity_streets) {
          if (meanDensityGlob > meanDensity_streets) {
            // Smaller than max density
            if (!(redTime > greenTime) && (redSum > greenSum) && (greenTime > delta)) {
              tl.decreaseGreenTimes(delta);
            } else if (!(redTime < greenTime) && (greenSum > redSum) &&
                       (redTime > delta)) {
              tl.increaseGreenTimes(delta);
            } else {
              tl.resetCycles();
            }
          } else {
            // Greater than max density
            if (!(redTime > greenTime) && (redSum > greenSum) && (greenTime > delta)) {
              tl.decreaseGreenTimes(delta * dyn_thresh);
            } else if (!(redTime < greenTime) && (greenSum > redSum) &&
                       (redTime > delta)) {
              tl.increaseGreenTimes(delta * dyn_thresh);
            } else {
              tl.resetCycles();
            }
          }
        }
      }
    }
    // Cleaning variables
    for (auto& [id, element] : m_streetTails) {
      element = 0;
    }
    m_previousOptimizationTime = this->m_time;
  }

  template <typename delay_t>
    requires(is_numeric_v<delay_t>)
  Measurement<double> RoadDynamics<delay_t>::meanTravelTime(bool clearData) {
    std::vector<double> travelTimes;
    if (!m_travelTimes.empty()) {
      if (clearData) {
        std::swap(travelTimes, m_travelTimes);
      } else {
        travelTimes.reserve(m_travelTimes.size());
        for (const auto& time : m_travelTimes) {
          travelTimes.push_back(time);
        }
      }
    }
    return Measurement<double>(travelTimes);
  }

  template <typename delay_t>
    requires(is_numeric_v<delay_t>)
  std::unordered_map<Id, std::array<double, 4>> RoadDynamics<delay_t>::turnProbabilities(
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

};  // namespace dsm
