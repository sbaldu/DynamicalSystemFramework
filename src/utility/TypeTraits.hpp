#ifndef TypeTraits_hpp
#define TypeTraits_hpp

#include <concepts>
#include <memory>
#include <type_traits>

namespace dmf {
  template <typename Id>
    requires std::unsigned_integral<Id>
  class Node;

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  class Street;

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

  // define is_numeric_v type trait
  template <typename T>
  inline constexpr bool is_numeric_v =
      (std::is_integral_v<T> || std::is_floating_point_v<T>)&&!std::is_same_v<T, bool> &&
      !std::is_same_v<T, char>;

};  // namespace dmf

#endif
