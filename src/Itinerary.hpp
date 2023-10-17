
#ifndef Itinerary_hpp
#define Itinerary_hpp

#include <concepts>
#include <utility>

namespace dmf {

  template <typename Id>
    requires std::unsigned_integral<Id>
  class Itinerary {
  private:
    std::pair<Id, Id> m_trip;

  public:
    Itinerary(Id source, Id destination) : m_trip{std::make_pair(source, destination)} {}
    explicit Itinerary(std::pair<Id, Id> trip) : m_trip{std::move(trip)} {}

    // Setters
    void set_source(Id source) { m_trip.first = source; }
    void set_destination(Id destination) { m_trip.second = destination; }

    // Getters
    Id source() const { return m_trip.first; }
    Id destination() const { return m_trip.second; }
  };
};  // namespace dmf

#endif
