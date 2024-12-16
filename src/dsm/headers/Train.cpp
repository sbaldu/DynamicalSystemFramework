#include "Train.hpp"

namespace dsm {
  Train::Train(Id id,
               std::vector<Id> const& trip,
               Id srcNodeId,
               train_t type,
               std::vector<std::pair<Time, Time>> const& schedule)
      : Agent<Delay>(id, trip, srcNodeId), m_type(type), m_schedule(schedule) {
    if (m_schedule.size() != this->m_trip.size()) {
      throw std::invalid_argument(
          buildLog("The schedule must have the same size as the trip"));
    }
  }

  void Train::addDelay(Time delay) { m_delays.emplace_back(delay); }

  void Train::reset() {
    Agent<Delay>::reset();
    m_delays.clear();
  }

  train_t Train::type() const { return m_type; }

  std::vector<std::pair<Time, Time>> const& Train::schedule() const { return m_schedule; }

  std::vector<Time> const& Train::delays() const { return m_delays; }
}  // namespace dsm