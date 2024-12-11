#pragma once

#include "Node.hpp"

#include <map>

namespace dsm {
  class Station : public Node {
  private:
    Delay m_managementTime;
    std::multimap<train_t, Id, std::greater<train_t>> m_trains;

  public:
    Station(Id id, Delay managementTime);
    Station(Id id, std::pair<double, double> coords, Delay managementTime);
    Station(Station const& other);

    void enqueue(Id trainId, train_t trainType);

    Id dequeue();

    Delay managementTime() const;
    double density() const final;
    bool isFull() const final;
    bool isStation() const noexcept final;
  };
}  // namespace dsm