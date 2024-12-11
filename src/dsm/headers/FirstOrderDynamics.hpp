#pragma once

#include "RoadDynamics.hpp"

namespace dsm {
  template <typename delay_t>
    requires(std::unsigned_integral<delay_t>)
  class FirstOrderDynamics : public RoadDynamics<delay_t> {
    double m_speedFluctuationSTD;

  public:
    /// @brief Construct a new First Order Dynamics object
    /// @param graph, The graph representing the network
    FirstOrderDynamics(Graph& graph, const unsigned int seed = 69)
        : RoadDynamics<delay_t>(graph, seed), m_speedFluctuationSTD{0.} {};
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

  template <typename delay_t>
    requires(std::unsigned_integral<delay_t>)
  void FirstOrderDynamics<delay_t>::setAgentSpeed(Size agentId) {
    const auto& agent{this->m_agents[agentId]};
    const auto& street{this->m_graph.streetSet()[agent->streetId().value()]};
    double speed{street->maxSpeed() *
                 (1. - this->m_minSpeedRateo * street->density(true))};
    if (m_speedFluctuationSTD > 0.) {
      std::normal_distribution<double> speedDist{speed, speed * m_speedFluctuationSTD};
      speed = speedDist(this->m_generator);
    }
    speed < 0. ? agent->setSpeed(street->maxSpeed() * (1. - this->m_minSpeedRateo))
               : agent->setSpeed(speed);
  }

  template <typename delay_t>
    requires(std::unsigned_integral<delay_t>)
  void FirstOrderDynamics<delay_t>::setSpeedFluctuationSTD(double speedFluctuationSTD) {
    if (speedFluctuationSTD < 0.) {
      throw std::invalid_argument(
          buildLog("The speed fluctuation standard deviation must be positive."));
    }
    m_speedFluctuationSTD = speedFluctuationSTD;
  }

  template <typename delay_t>
    requires(std::unsigned_integral<delay_t>)
  double FirstOrderDynamics<delay_t>::streetMeanSpeed(Id streetId) const {
    const auto& street{this->m_graph.streetSet().at(streetId)};
    if (street->nAgents() == 0) {
      return street->maxSpeed();
    }
    double meanSpeed{0.};
    Size n{0};
    if (street->nExitingAgents() == 0) {
      n = static_cast<Size>(street->waitingAgents().size());
      double alpha{this->m_minSpeedRateo / street->capacity()};
      meanSpeed = street->maxSpeed() * n * (1. - 0.5 * alpha * (n - 1.));
    } else {
      for (const auto& agentId : street->waitingAgents()) {
        meanSpeed += this->m_agents.at(agentId)->speed();
        ++n;
      }
      for (auto const& queue : street->exitQueues()) {
        for (const auto& agentId : queue) {
          meanSpeed += this->m_agents.at(agentId)->speed();
          ++n;
        }
      }
    }
    const auto& node = this->m_graph.nodeSet().at(street->nodePair().second);
    if (node->isIntersection()) {
      auto& intersection = dynamic_cast<Intersection&>(*node);
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

  template <typename delay_t>
    requires(std::unsigned_integral<delay_t>)
  Measurement<double> FirstOrderDynamics<delay_t>::streetMeanSpeed() const {
    if (this->m_agents.size() == 0) {
      return Measurement(0., 0.);
    }
    std::vector<double> speeds;
    speeds.reserve(this->m_graph.streetSet().size());
    for (const auto& [streetId, street] : this->m_graph.streetSet()) {
      speeds.push_back(this->streetMeanSpeed(streetId));
    }
    return Measurement<double>(speeds);
  }
  template <typename delay_t>
    requires(std::unsigned_integral<delay_t>)
  Measurement<double> FirstOrderDynamics<delay_t>::streetMeanSpeed(double threshold,
                                                                   bool above) const {
    if (this->m_agents.size() == 0) {
      return Measurement(0., 0.);
    }
    std::vector<double> speeds;
    speeds.reserve(this->m_graph.streetSet().size());
    for (const auto& [streetId, street] : this->m_graph.streetSet()) {
      if (above) {
        if (street->density(true) > threshold) {
          speeds.push_back(this->streetMeanSpeed(streetId));
        }
      } else {
        if (street->density(true) < threshold) {
          speeds.push_back(this->streetMeanSpeed(streetId));
        }
      }
    }
    return Measurement<double>(speeds);
  }
}  // namespace dsm