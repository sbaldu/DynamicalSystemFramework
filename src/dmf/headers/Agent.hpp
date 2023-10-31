
#ifndef Agent_hpp
#define Agent_hpp

#include "Itinerary.hpp"
#include "SparseMatrix.hpp"
#include "../utility/TypeTraits/is_numeric.hpp"

namespace dmf {

  template <typename Id, typename Weight>
    requires(std::unsigned_integral<Id> && is_numeric_v<Weight>)
  class Agent {
  private:
    Id m_index;
    Id m_position;
    Id m_previousPosition;
    Itinerary<Id> m_itinerary;
    SparseMatrix<Id, Weight> m_transitionMatrix;

  public:
    Agent() = default;
    Agent(Id index, Id position);
    Agent(Id index, Id position, Itinerary<Id> itinerary);
    Agent(Id index, Id position, Itinerary<Id> itinerary, SparseMatrix<Id, Weight> matrix);

    // Setters
    void setPosition(Id position);
    void setItinerary(Itinerary<Id> itinerary);
    void setTransitionMatrix(SparseMatrix<Id, Weight> matrix);

    // Getters
    int index() const;
    int position() const;
    int previousPosition() const;
    const Itinerary<Id>& itinerary() const;
    const SparseMatrix<Id, Weight>& transitionMatrix() const;
  };

  template <typename Id, typename Weight>
    requires(std::unsigned_integral<Id> && is_numeric_v<Weight>)
  Agent<Id, Weight>::Agent(Id index, Id position)
      : m_index{index}, m_position{position}, m_previousPosition{position} {}

  template <typename Id, typename Weight>
    requires(std::unsigned_integral<Id> && is_numeric_v<Weight>)
  Agent<Id, Weight>::Agent(Id index, Id position, Itinerary<Id> itinerary)
      : m_index{index},
        m_position{position},
        m_previousPosition{position},
        m_itinerary{std::move(itinerary)} {}

  template <typename Id, typename Weight>
    requires(std::unsigned_integral<Id> && is_numeric_v<Weight>)
  Agent<Id, Weight>::Agent(Id index, Id position, Itinerary<Id> itinerary, SparseMatrix<Id, Weight> matrix)
      : m_index{index},
        m_position{position},
        m_previousPosition{position},
        m_itinerary{std::move(itinerary)},
        m_transitionMatrix{std::move(matrix)} {}

  // Setters
  template <typename Id, typename Weight>
    requires(std::unsigned_integral<Id> && is_numeric_v<Weight>)
  void Agent<Id, Weight>::setPosition(Id position) {
    m_position = position;
  }
  template <typename Id, typename Weight>
    requires(std::unsigned_integral<Id> && is_numeric_v<Weight>)
  void Agent<Id, Weight>::setItinerary(Itinerary<Id> itinerary) {
    m_itinerary = std::move(itinerary);
  }
  template <typename Id, typename Weight>
    requires(std::unsigned_integral<Id> && is_numeric_v<Weight>)
  void Agent<Id, Weight>::setTransitionMatrix(SparseMatrix<Id, Weight> matrix) {
    m_transitionMatrix = std::move(matrix);
  }

  // Getters
  template <typename Id, typename Weight>
    requires(std::unsigned_integral<Id> && is_numeric_v<Weight>)
  int Agent<Id, Weight>::index() const {
    return m_index;
  }
  template <typename Id, typename Weight>
    requires(std::unsigned_integral<Id> && is_numeric_v<Weight>)
  int Agent<Id, Weight>::position() const {
    return m_position;
  }
  template <typename Id, typename Weight>
    requires(std::unsigned_integral<Id> && is_numeric_v<Weight>)
  int Agent<Id, Weight>::previousPosition() const {
    return m_previousPosition;
  }
  template <typename Id, typename Weight>
    requires(std::unsigned_integral<Id> && is_numeric_v<Weight>)
  const Itinerary<Id>& Agent<Id, Weight>::itinerary() const {
    return m_itinerary;
  }
  template <typename Id, typename Weight>
    requires(std::unsigned_integral<Id> && is_numeric_v<Weight>)
  const SparseMatrix<Id, Weight>& Agent<Id, Weight>::transitionMatrix() const {
    return m_transitionMatrix;
  }
};  // namespace dmf

#endif
