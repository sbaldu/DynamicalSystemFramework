#ifndef is_agent_hpp
#define is_agent_hpp

#include <concepts>
#include <memory>
#include <type_traits>

namespace dmf {
  template <typename Id>
    requires std::unsigned_integral<Id>
  class Agent;

  // Alias for shared pointers
  template <typename T>
  using shared = std::shared_ptr<T>;

  // define is_node type trait
  template <typename T>
  struct is_agent : std::false_type {};

  template <typename Id>
  struct is_agent<Agent<Id>> : std::true_type {};

  template <typename Id>
  struct is_agent<shared<Agent<Id>>> : std::true_type {};

  template <typename T>
  inline constexpr bool is_agent_v = is_agent<T>::value;

};  // namespace dmf

#endif
