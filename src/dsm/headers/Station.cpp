#include "Station.hpp"

namespace dsm {
  Station::Station(Id id, Delay managementTime)
      : Node(id), m_managementTime{managementTime}, m_lastEventTime{0} {}

  Station::Station(Id id, std::pair<double, double> coords, Delay managementTime)
      : Node(id, coords), m_managementTime{managementTime}, m_lastEventTime{0} {}

  Station::Station(Node const& node, Delay managementTime)
      : Node(node), m_managementTime{managementTime}, m_lastEventTime{0} {}

  Station::Station(Station const& other)
      : Node(other),
        m_managementTime{other.m_managementTime},
        m_lastEventTime{other.m_lastEventTime},
        m_trains{other.m_trains} {}

  void Station::enqueue(Id trainId, train_t trainType) {
    m_trains.emplace(trainType, trainId);
  }

  std::optional<Id> Station::dequeue() {
    if (m_trains.empty()) {
      return std::nullopt;
    }
    auto it = m_trains.begin();
    Id trainId = it->second;
    m_trains.erase(it);
    return trainId;
  }

  Delay Station::managementTime() const { return m_managementTime; }

  double Station::density() const {
    return static_cast<double>(m_trains.size()) / m_capacity;
  }

  bool Station::isFull() const { return m_trains.size() >= m_capacity; }

  bool Station::isActive(Time currentTime) const {
    return currentTime - m_lastEventTime >= m_managementTime;
  }

  bool Station::isStation() const noexcept { return true; }
}  // namespace dsm