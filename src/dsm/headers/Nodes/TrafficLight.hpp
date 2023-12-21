
#ifndef TrafficLight_hpp
#define TrafficLight_hpp

#include <concepts>
#include <type_traits>

#include "Node.hpp"

namespace dsm {
  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>
  class TrafficLight : public Node<Id, Size> {
  private:
  public:
  };

  class TrafficLight : public Node<Id, Size> {
  private:
    std::optional<std::pair<Delay, Delay>> m_delay;
    Delay m_counter;

  public:
    TrafficLight() = default;
    /// @brief Construct a new TrafficLight object
    /// @param id The node's id
    TrafficLight(Id id);

    /// @brief Set the node's delay
    /// @param delay The node's delay
    void setDelay(Delay delay);
    /// @brief Set the node's delay
    /// @param delay The node's delay
    void setDelay(std::pair<Delay, Delay> delay);
    /// @brief Set the node's phase
    /// @param phase The node's phase
    /// @throw std::runtime_error if the delay is not set
    void setPhase(Delay phase);
    /// @brief Increase the node's counter
    /// @details This function is used to increase the node's counter
    ///          when the simulation is running. It automatically resets the counter
    ///          when it reaches the double of the delay value.
    /// @throw std::runtime_error if the delay is not set
    void increaseCounter();

    /// @brief Get the node's delay
    /// @return std::optional<Delay> The node's delay
    std::optional<Delay> delay() const;
    Delay counter() const { return m_counter; }
    /// @brief Returns true if the traffic light is green
    /// @return bool True if the traffic light is green
    bool isGreen() const;
    bool isGreen(Id streetId) const;
  };

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>
  TrafficLight<Id, Size, Delay>::TrafficLight(Id id) : Node<Id, Size>{id}, m_counter{0} {}

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>
  void TrafficLight<Id, Size, Delay>::setDelay(Delay delay) {
    m_delay = std::make_pair(delay, delay);
  }
  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>
  void TrafficLight<Id, Size, Delay>::setDelay(std::pair<Delay, Delay> delay) {
    m_delay = std::move(delay);
  }
  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>
  void TrafficLight<Id, Size, Delay>::setPhase(Delay phase) {
    if (!m_delay.has_value()) {
      std::string errorMsg{"Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
                           "TrafficLight's delay is not set"};
      throw std::runtime_error(errorMsg);
    }
    if (phase > m_delay.value().first + m_delay.value().second) {
      phase -= m_delay.value().first + m_delay.value().second;
    }
    phase == 0 ? m_counter = 0 : m_counter = m_delay.value().first % phase;  //fwefbewbfw
  }
  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>
  void TrafficLight<Id, Size, Delay>::increaseCounter() {
    if (!m_delay.has_value()) {
      std::string errorMsg{"Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
                           "TrafficLight's delay is not set"};
      throw std::runtime_error(errorMsg);
    }
    ++m_counter;
    if (m_counter == m_delay.value().first + m_delay.value().second) {
      m_counter = 0;
    }
  }

  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>
  std::optional<Delay> TrafficLight<Id, Size, Delay>::delay() const {
    return m_delay;
  }
  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>
  bool TrafficLight<Id, Size, Delay>::isGreen() const {
    if (!m_delay.has_value()) {
      std::string errorMsg{"Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
                           "TrafficLight's delay is not set"};
      throw std::runtime_error(errorMsg);
    }
    return m_counter < m_delay.value().first;
  }
  template <typename Id, typename Size, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>
  bool TrafficLight<Id, Size, Delay>::isGreen(Id streetId) const {
    if (!m_delay.has_value()) {
      std::string errorMsg{"Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
                           "TrafficLight's delay is not set"};
      throw std::runtime_error(errorMsg);
    }
    if (this->isGreen()) {
      return this->streetPriorities().at(streetId) > 0;
    }
    return this->streetPriorities().at(streetId) < 0;
  }

};  // namespace dsm

#endif
