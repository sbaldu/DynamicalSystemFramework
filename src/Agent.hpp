
#ifndef Agent_hpp
#define Agent_hpp

#include "Itinerary.hpp"
#include "SparseMatrix.hpp"
#include "utility/TypeTraits.hpp"
#include <vector>

namespace dmf {

  template <typename Id>
    requires std::unsigned_integral<Id>
  class Agent {
  private:
    Id m_index;
    Id m_position;
    Id m_previousPosition;
    static std::vector<Itinerary<Id>> m_itineraries;
    Id m_itineraryIndex;

  public:
    Agent() = default;
    Agent(Id index, Id itinerary);
    Agent(Id index, Id position, Id itinerary);

    // Setters
    static void addItinerary(Itinerary<Id> itinerary);
    void setPosition(Id position);
    void setItinerary(Itinerary<Id> itinerary);

    // Getters
    int index() const;
    int position() const;
    int previousPosition() const;
    const Itinerary<Id>& itinerary() const;
  };

  template <typename Id>
    requires std::unsigned_integral<Id>
std::vector<Itinerary<Id>> Agent<Id>::m_itineraries;

  template <typename Id>
    requires std::unsigned_integral<Id>
  Agent<Id>::Agent(Id index, Id itinerary)
      : m_index{index}, m_position{m_itineraries.at(itinerary).source()}, m_previousPosition{m_itineraries.at(itinerary).source()}, m_itineraryIndex{itinerary} {}

  template <typename Id>
    requires std::unsigned_integral<Id>
  Agent<Id>::Agent(Id index, Id position, Id itinerary)
      : m_index{index},
        m_position{position},
        m_previousPosition{position},
        m_itineraryIndex{itinerary} {}

  // Setters
  template <typename Id>
    requires std::unsigned_integral<Id>
  void addItinerary(Itinerary<Id> itinerary) {
    m_itineraries.push_back(std::move(itinerary));
  }
  template <typename Id>
    requires std::unsigned_integral<Id>
  void Agent<Id>::setPosition(Id position) {
    m_position = position;
  }
  template <typename Id>
    requires std::unsigned_integral<Id>
  void Agent<Id>::setItinerary(Itinerary<Id> itinerary) {
    m_itineraryIndex = itinerary;
  }

  // Getters
  template <typename Id>
    requires std::unsigned_integral<Id>
  int Agent<Id>::index() const {
    return m_index;
  }
  template <typename Id>
    requires std::unsigned_integral<Id>
  int Agent<Id>::position() const {
    return m_position;
  }
  template <typename Id>
    requires std::unsigned_integral<Id>
  int Agent<Id>::previousPosition() const {
    return m_previousPosition;
  }
  template <typename Id>
    requires std::unsigned_integral<Id>
  const Itinerary<Id>& Agent<Id>::itinerary() const {
    return m_itineraries.at(m_itineraryIndex);
  }
};  // namespace dmf

#endif
