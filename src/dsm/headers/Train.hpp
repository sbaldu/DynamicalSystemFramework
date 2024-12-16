#pragma once

#include "Agent.hpp"
#include "../utility/Typedef.hpp"

#include <vector>

namespace dsm {
  class Train : public Agent<Delay> {
  private:
    train_t m_type;
    std::vector<std::pair<Time, Time>> m_schedule;
    std::vector<Time> m_delays;

  public:
    Train(Id id,
          std::vector<Id> const& trip,
          Id srcNodeId,
          train_t type,
          std::vector<std::pair<Time, Time>> const& schedule);

    void addDelay(Time delay);

    void reset() override;

    train_t type() const;

    std::vector<std::pair<Time, Time>> const& schedule() const;

    std::vector<Time> const& delays() const;
  };
}  // namespace dsm