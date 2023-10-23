
#ifndef Itinerary_hpp
#define Itinerary_hpp

#include "SparseMatrix.hpp"

#include <concepts>
#include <utility>

namespace dmf {

  template <typename Id>
  requires std::unsigned_integral<Id>
  class Itinerary {
  private:
    std::pair<Id, Id> m_trip;
    SparseMatrix<Id, bool> m_path;

  public:
    Itinerary() = default;
    Itinerary(Id source, Id destination);
    explicit Itinerary(std::pair<Id, Id> trip) : m_trip{std::move(trip)} {}
    Itinerary(Id source, Id destination, SparseMatrix<Id, bool> path);
    Itinerary(std::pair<Id, Id> trip, SparseMatrix<Id, bool> path);

    // Setters
    void setSource(Id source);
    void setDestination(Id destination);
    void setPath(SparseMatrix<Id, bool> path);

    // Getters
    Id source() const;
    Id destination() const;
    std::pair<Id, Id> trip() const;
    SparseMatrix<Id, bool> path() const;
  };
  template <typename Id>
  requires std::unsigned_integral<Id> Itinerary<Id>::Itinerary(Id source, Id destination)
      : m_trip{std::make_pair(source, destination)} {}
  template <typename Id>
  requires std::unsigned_integral<Id> Itinerary<Id>::Itinerary(Id source,
                                                               Id destination,
                                                               SparseMatrix<Id, bool> path)
      : m_trip{std::make_pair(source, destination)}, m_path{std::move(path)} {}
  template <typename Id>
  requires std::unsigned_integral<Id> Itinerary<Id>::Itinerary(std::pair<Id, Id> trip,
                                                               SparseMatrix<Id, bool> path)
      : m_trip{std::move(trip)}, m_path{std::move(path)} {}

  //Setters
  template <typename Id>
  requires std::unsigned_integral<Id>
  void Itinerary<Id>::setSource(Id source) { m_trip.first = source; }
  template <typename Id>
  requires std::unsigned_integral<Id>
  void Itinerary<Id>::setDestination(Id destination) { m_trip.second = destination; }
  template <typename Id>
  requires std::unsigned_integral<Id>
  void Itinerary<Id>::setPath(SparseMatrix<Id, bool> path) { m_path = std::move(path); }

  //Getters
  template <typename Id>
  requires std::unsigned_integral<Id> Id Itinerary<Id>::source()
  const { return m_trip.first; }
  template <typename Id>
  requires std::unsigned_integral<Id> Id Itinerary<Id>::destination()
  const { return m_trip.second; }
  template <typename Id>
  requires std::unsigned_integral<Id> std::pair<Id, Id> Itinerary<Id>::trip()
  const { return m_trip; }
  template <typename Id>
  requires std::unsigned_integral<Id> SparseMatrix<Id, bool> Itinerary<Id>::path()
  const { return m_path; }

};  // namespace dmf

#endif
