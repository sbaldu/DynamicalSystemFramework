
#ifndef Agent_hpp
#define Agent_hpp

#include "Itinerary.hpp"
#include "SparseMatrix.hpp"
#include "utility/TypeTraits/is_numeric.hpp"

#include <stdexcept>
#include <string>
#include <limits>

namespace dmf {

  template <typename Id, typename Weight>
  requires(std::unsigned_integral<Id>&& is_numeric_v<Weight>) class Agent {
  private:
    Id m_index;
    Id m_position;
    Id m_previousPosition;
    Itinerary<Id> m_itinerary;
    double m_velocity;
    unsigned int m_time;
    SparseMatrix<Id, Weight> m_transitionMatrix;

  public:
    Agent() = default;
    Agent(Id index, Id position);
    Agent(Id index, Id position, Itinerary<Id> itinerary);
    Agent(Id index, Id position, Itinerary<Id> itinerary, SparseMatrix<Id, Weight> matrix);

    // Setters
    void setPosition(Id position);
    void setItinerary(Itinerary<Id> itinerary);
    void setVelocity(double velocity);
    void incrementTime();
    void incrementTime(unsigned int time);
    void resetTime();
    void setTransitionMatrix(SparseMatrix<Id, Weight> matrix);

    // Getters
    int index() const;
    int position() const;
    int previousPosition() const;
    const Itinerary<Id>& itinerary() const;
    double velocity() const;
    unsigned int time() const;
    const SparseMatrix<Id, Weight>& transitionMatrix() const;
  };

  template <typename Id, typename Weight>
  requires(std::unsigned_integral<Id>&& is_numeric_v<Weight>) Agent<Id, Weight>::Agent(Id index,
                                                                                       Id position)
      : m_index{index}, m_position{position}, m_previousPosition{position}, m_velocity{0.}, m_time{0} {}

  template <typename Id, typename Weight>
  requires(std::unsigned_integral<Id>&& is_numeric_v<Weight>) Agent<Id, Weight>::Agent(
      Id index, Id position, Itinerary<Id> itinerary)
      : m_index{index},
        m_position{position},
        m_previousPosition{position},
        m_itinerary{std::move(itinerary)},
        m_velocity{0.},
        m_time{0} {}

  template <typename Id, typename Weight>
  requires(std::unsigned_integral<Id>&& is_numeric_v<Weight>) Agent<Id, Weight>::Agent(
      Id index, Id position, Itinerary<Id> itinerary, SparseMatrix<Id, Weight> matrix)
      : m_index{index},
        m_position{position},
        m_previousPosition{position},
        m_itinerary{std::move(itinerary)},
        m_transitionMatrix{std::move(matrix)},
        m_velocity{0.},
        m_time{0} {}

  // Setters
  template <typename Id, typename Weight>
  requires(std::unsigned_integral<Id>&& is_numeric_v<Weight>) void Agent<Id, Weight>::setPosition(
      Id position) {
    m_position = position;
  }
  template <typename Id, typename Weight>
  requires(std::unsigned_integral<Id>&& is_numeric_v<Weight>) void Agent<Id, Weight>::setItinerary(
      Itinerary<Id> itinerary) {
    m_itinerary = std::move(itinerary);
  }
  template <typename Id, typename Weight>
  requires(std::unsigned_integral<Id>&& is_numeric_v<Weight>) void Agent<Id, Weight>::setVelocity(
      double velocity) {
    if (velocity < 0) {
      std::string errorMsg = "Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
                             "Velocity must be positive";
      throw std::invalid_argument(errorMsg);
    }
    m_velocity = velocity;
  }
  template <typename Id, typename Weight>
  requires(std::unsigned_integral<Id>&& is_numeric_v<Weight>) void Agent<Id, Weight>::incrementTime() {
    if (m_time == std::numeric_limits<unsigned int>::max()) {
      std::string errorMsg = "Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
                             "Time has reached its maximum value";
      throw std::overflow_error(errorMsg);
    }
    ++m_time;
  }
  template <typename Id, typename Weight>
  requires(std::unsigned_integral<Id>&& is_numeric_v<Weight>) void incrementTime(unsigned int time) {
    if (m_time + time < m_time) {
      std::string errorMsg = "Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
                             "Time has reached its maximum value";
      throw std::overflow_error(errorMsg);
    }
    m_time += time;
  }
  template <typename Id, typename Weight>
  requires(std::unsigned_integral<Id>&& is_numeric_v<Weight>) void Agent<Id, Weight>::setTransitionMatrix(
      SparseMatrix<Id, Weight> matrix) {
    m_transitionMatrix = std::move(matrix);
  }

  // Getters
  template <typename Id, typename Weight>
  requires(std::unsigned_integral<Id>&& is_numeric_v<Weight>) int Agent<Id, Weight>::index() const {
    return m_index;
  }
  template <typename Id, typename Weight>
  requires(std::unsigned_integral<Id>&& is_numeric_v<Weight>) int Agent<Id, Weight>::position() const {
    return m_position;
  }
  template <typename Id, typename Weight>
  requires(
      std::unsigned_integral<Id>&& is_numeric_v<Weight>) int Agent<Id, Weight>::previousPosition() const {
    return m_previousPosition;
  }
  template <typename Id, typename Weight>
  requires(std::unsigned_integral<Id>&& is_numeric_v<Weight>) double Agent<Id, Weight>::velocity() const {
    return m_velocity;
  }
  template <typename Id, typename Weight>
  requires(std::unsigned_integral<Id>&& is_numeric_v<Weight>) unsigned int Agent<Id, Weight>::time() const {
    return m_time;
  }
  template <typename Id, typename Weight>
  requires(std::unsigned_integral<Id>&& is_numeric_v<Weight>)
      const Itinerary<Id>& Agent<Id, Weight>::itinerary() const {
    return m_itinerary;
  }
  template <typename Id, typename Weight>
  requires(std::unsigned_integral<Id>&& is_numeric_v<Weight>) const
      SparseMatrix<Id, Weight>& Agent<Id, Weight>::transitionMatrix() const {
    return m_transitionMatrix;
  }
};  // namespace dmf

#endif
