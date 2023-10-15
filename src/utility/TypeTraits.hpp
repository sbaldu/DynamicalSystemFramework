#ifndef TypeTraits_hpp
#define TypeTraits_hpp

#include <concepts>
#include <memory>
#include <type_traits>

#include "../Node.hpp"
#include "../Street.hpp"

namespace dmf {
  // Alias for shared pointers
  template <typename T>
  using shared = std::shared_ptr<T>;

  template <typename T>
  struct is_node : std::false_type {};

  template <typename Id>
  struct is_node<Node<Id>> : std::true_type {};

  template <typename Id>
  struct is_node<shared<Node<Id>>> : std::true_type {};

  template <typename T>
  inline constexpr bool is_node_v = is_node<T>::value;

  template <typename T>
  struct is_street : std::false_type {};

  template <typename Id, typename Size>
  struct is_street<Street<Id, Size>> : std::true_type {};

  template <typename Id, typename Size>
  struct is_street<shared<Street<Id, Size>>> : std::true_type {};

  template <typename T>
  inline constexpr bool is_street_v = is_street<T>::value;

};  // namespace dmf

#endif
