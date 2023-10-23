
#ifndef Agent_hpp
#define Agent_hpp

#include "Itinerary.hpp"
#include "utility/TypeTraits/is_numeric.hpp"

#include <stdexcept>
#include <string>
#include <limits>

namespace dmf {

  template <typename Id>
  requires std::unsigned_integral<Id>
  class Agent {
  private:
    Id m_index;
    Id m_position;
    Id m_previousPosition;
    Itinerary<Id> m_itinerary;
    double m_speed;
    unsigned int m_time;

  public:
    Agent() = default;
    Agent(Id index, Id position);
    Agent(Id index, Id position, Itinerary<Id> itinerary);

    // Setters
    void setPosition(Id position);
    void setItinerary(Itinerary<Id> itinerary);
    void setSpeed(double speed);
    void incrementTime();
    void incrementTime(unsigned int time);
    void resetTime();

    // Getters
    int index() const;
    int position() const;
    int previousPosition() const;
    const Itinerary<Id>& itinerary() const;
    double speed() const;
    unsigned int time() const;
  };

  template <typename Id>
  requires std::unsigned_integral<Id> Agent<Id>::Agent(Id index, Id position)
      : m_index{index}, m_position{position}, m_previousPosition{position}, m_speed{0.}, m_time{0} {}

  template <typename Id>
  requires std::unsigned_integral<Id> Agent<Id>::Agent(Id index, Id position, Itinerary<Id> itinerary)
      : m_index{index},
        m_position{position},
        m_previousPosition{position},
        m_itinerary{std::move(itinerary)},
        m_speed{0.},
        m_time{0} {}

  // Setters
  template <typename Id>
  requires std::unsigned_integral<Id>
  void Agent<Id>::setPosition(Id position) { m_position = position; }
  template <typename Id>
  requires std::unsigned_integral<Id>
  void Agent<Id>::setItinerary(Itinerary<Id> itinerary) { m_itinerary = std::move(itinerary); }
  template <typename Id>
  requires std::unsigned_integral<Id>
  void Agent<Id>::setSpeed(double speed) {
    if (speed < 0) {
      std::string errorMsg = "Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
                             "Speed must be positive";
      throw std::invalid_argument(errorMsg);
    }
    m_speed = speed;
  }
  template <typename Id>
  requires std::unsigned_integral<Id>
  void Agent<Id>::incrementTime() {
    if (m_time == std::numeric_limits<unsigned int>::max()) {
      std::string errorMsg = "Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
                             "Time has reached its maximum value";
      throw std::overflow_error(errorMsg);
    }
    ++m_time;
  }
  template <typename Id>
  requires std::unsigned_integral<Id>
  void Agent<Id>::incrementTime(unsigned int time) {
    if (m_time + time < m_time) {
      std::string errorMsg = "Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
                             "Time has reached its maximum value";
      throw std::overflow_error(errorMsg);
    }
    m_time += time;
  }

  // Getters
  template <typename Id>
  requires std::unsigned_integral<Id>
  int Agent<Id>::index() const { return m_index; }
  template <typename Id>
  requires std::unsigned_integral<Id>
  int Agent<Id>::position() const { return m_position; }
  template <typename Id>
  requires std::unsigned_integral<Id>
  int Agent<Id>::previousPosition() const { return m_previousPosition; }
  template <typename Id>
  requires std::unsigned_integral<Id>
  double Agent<Id>::speed() const { return m_speed; }
  template <typename Id>
  requires std::unsigned_integral<Id>
  unsigned int Agent<Id>::time() const { return m_time; }
  template <typename Id>
  requires std::unsigned_integral<Id>
  const Itinerary<Id>& Agent<Id>::itinerary() const { return m_itinerary; }
};  // namespace dmf

#endif
