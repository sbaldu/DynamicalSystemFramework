
#ifndef VehicleCounter_hpp
#define VehicleCounter_hpp

#include <concepts>

#include "../../utility/TypeTraits/is_node.hpp"

namespace dsm {

  template <typename Base>
	requires is_node_v<Base>
  class VehicleCounter : public Base {
	private:
	  Base::Size_t m_vehicleCount;

	public:
	  VehicleCounter() : m_vehicleCount{} {}

	  void incrementVehicleCount() { ++m_vehicleCount; }
	  Base::Size_t vehicleCount() const { return m_vehicleCount; }
  };

};  // namespace dsm

#endif
