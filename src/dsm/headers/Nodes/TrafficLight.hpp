
#ifndef TrafficLight_hpp
#define TrafficLight_hpp

#include <concepts>
#include <type_traits>
#include <optional>

#include "Node.hpp"

namespace dsm {

  template <typename Delay>
  struct TrafficLightCycle {
    Delay green;
    Delay red;

    TrafficLightCycle() = delete;
    TrafficLightCycle(Delay green, Delay red) : green{green}, red{red} {}
    TrafficLightCycle(const std::pair<Delay, Delay>& pair) : green{pair.first}, red{pair.second} {}
  };

  template <typename Id, typename Size, typename Priority, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>
  class TrafficLight : public Node<Id, Size, Priority> {
  private:
    std::optional<TrafficLightCycle<Delay>> m_delay;
    Delay m_counter;

  public:
    TrafficLight() = delete;
    /// @brief Construct a new TrafficLight object
    /// @param id The node's id
    explicit TrafficLight(Id id);

    /// @brief Set the node's delay
    /// @param delay The node's delay
    void setDelay(Delay delay);
    /// @brief Set the node's delay
    /// @param delay The node's delay
    void setDelay(const std::pair<Delay, Delay>& delay);
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
    /// @brief Check if the node's queue is full or the light is red
    /// @return true if the node's queue is full or if the node's counter is less than the first value of the delay
    ///         false otherwise
    /// @throw std::runtime_error if the delay is not set
    bool isFull() const override;
    /// @brief Check if the node's queue is full or the light is red
    /// @details The light is red if the node's counter is less than the first value of the delay for the streets with negative priority
    ///          and if the node's counter is greater than the first value of the delay for the streets with positive priority
    /// @param streetId The id of the street that is connected to the node
    /// @return true if the node's queue is full or if the street has red light
    ///         false otherwise
    /// @throw std::runtime_error if the delay is not set
    bool isFull(Id streetId) const override;
  };

  template <typename Id, typename Size, typename Priority, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>
  TrafficLight<Id, Size, Priority, Delay>::TrafficLight(Id id) : Node<Id, Size>{id}, m_counter{0} {}

  template <typename Id, typename Size, typename Priority, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>
  void TrafficLight<Id, Size, Priority, Delay>::setDelay(Delay delay) {
    m_delay = std::make_pair(delay, delay);
  }
  template <typename Id, typename Size, typename Priority, typename Delay>
    requires std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>
  void TrafficLight<Id, Size, Priority, Delay>::setDelay(const std::pair<Delay, Delay>& delay) {
     m_delay = std::move(delay); }
    template <typename Id, typename Size, typename Priority, typename Delay>
      requires std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>
    void TrafficLight<Id, Size, Priority, Delay>::setPhase(Delay phase) {
      if (!m_delay.has_value()) {
        std::string errorMsg{"Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
                             "TrafficLight's delay is not set"};
        throw std::runtime_error(errorMsg);
      }
      if (phase > m_delay.value().first + m_delay.value().second) {
        phase -= m_delay.value().first + m_delay.value().second;
      }
      phase == 0 ? m_counter = 0 : m_counter = m_delay.value().first % phase;
    }
    template <typename Id, typename Size, typename Priority, typename Delay>
      requires std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>
    void TrafficLight<Id, Size, Priority, Delay>::increaseCounter() {
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

    template <typename Id, typename Size, typename Priority, typename Delay>
      requires std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>
    std::optional<Delay> TrafficLight<Id, Size, Priority, Delay>::delay() const {
      return m_delay;
    }

    template <typename Id, typename Size, typename Priority, typename Delay>
      requires std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>
    bool TrafficLight<Id, Size, Priority, Delay>::isFull() const {
      if (!m_delay.has_value()) {
        std::string errorMsg{"Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
                             "TrafficLight's delay is not set"};
        throw std::runtime_error(errorMsg);
      }
      if (m_capacity == m_queue.size()) {
        return true;
      }
      return m_counter < m_delay.value().first;
    }
    template <typename Id, typename Size, typename Priority, typename Delay>
      requires std::unsigned_integral<Id> && std::unsigned_integral<Size> && std::unsigned_integral<Delay>
    bool TrafficLight<Id, Size, Priority, Delay>::isFull(Id streetId) const {
      if (!m_delay.has_value()) {
        std::string errorMsg{"Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
                             "TrafficLight's delay is not set"};
        throw std::runtime_error(errorMsg);
      }
      if (m_capacity == m_queue.size()) {
      return true;
    }
        if (m_counter < m_delay.value().first) {
          return this->streetPriorities()[streetId] < 0;
        }
        return this->streetPriorities()[streetId] > 0;
      }
    };  // namespace dsm

#endif
