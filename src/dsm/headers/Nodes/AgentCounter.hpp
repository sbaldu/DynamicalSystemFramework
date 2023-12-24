
#ifndef AgentCounter_hpp
#define AgentCounter_hpp

#include <concepts>

#include "../../utility/TypeTraits/is_node.hpp"

namespace dsm {

  template <typename Base>
	requires is_node_v<Base>
  class AgentCounter : public Base {
	private:
	  Base::Size_t m_agentCount;

	public:
	  AgentCounter() : m_agentCount{} {}

	  void incrementVehicleCount() { ++m_agentCount; }
	  Base::Size_t vehicleCount() const { return m_agentCount; }
  };

};  // namespace dsm

#endif
