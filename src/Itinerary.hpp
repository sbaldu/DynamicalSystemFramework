
#ifndef Itinerary_hpp
#define Itinerary_hpp

#include <concepts>
#include <utility>
#include "SparseMatrix.hpp"

namespace dmf {

  template <typename Id>
	  requires std::unsigned_integral<Id>
  class Itinerary {
  private:
    std::pair<Id, Id> m_trip;
    SparseMatrix<Id, bool> m_transitionMatrix;

  public:
    Itinerary(Id source, Id destination) : m_trip{std::make_pair(source, destination)} {}
    Itinerary(std::pair<Id, Id> trip) : m_trip{std::move(trip)} {}

    // Setters
    void set_source(Id source) { m_trip.first = source; }
    void set_destination(Id destination) { m_trip.second = destination; }
    void setTransitionMatrix(SparseMatrix<Id, bool> matrix);

    // Getters
    Id source() const { return m_trip.first; }
    Id destination() const { return m_trip.second; }
    const SparseMatrix<Id, bool>& transitionMatrix() const;
  };

  template <typename Id>
    requires std::unsigned_integral<Id>
  void Itinerary<Id>::setTransitionMatrix(SparseMatrix<Id, bool> matrix) {
    m_transitionMatrix = std::move(matrix);
  }
  template <typename Id>
    requires std::unsigned_integral<Id>
  const SparseMatrix<Id, bool>& Itinerary<Id>::transitionMatrix() const {
    return m_transitionMatrix;
  }
};  // namespace dmf

#endif
