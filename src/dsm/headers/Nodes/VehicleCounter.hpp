
#ifndef VehicleCounter_hpp
#define VehicleCounter_hpp

#include <concepts>

#include "../../utility/TypeTraits/is_node.hpp"
#include "../../utility/TypeTraits/is_street.hpp"

namespace dsm {

  template <typename Base>
    requires is_node_v<Base> || is_street_v<Base>
  class VehicleCounter : public Base {
  private:
    Base::Size_t m_vehicleCount;

  public:
    VehicleCounter() : m_vehicleCount{} {}

    void incrementVehicleCount() { ++m_vehicleCount; }
    Base::Size_t vehicleCount(bool resetCounter = true) const {
      Base::Size_t count{m_vehicleCount};
      if (resetCounter) {
        m_vehicleCount = 0;
      }

      return count;
    }
  };

};  // namespace dsm

#endif
