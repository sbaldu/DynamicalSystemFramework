#ifndef is_street_hpp
#define is_street_hpp

#include <concepts>
#include <memory>
#include <type_traits>

namespace dmf {
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  class Street;

  // Alias for shared pointers
  template <typename T>
  using shared = std::shared_ptr<T>;

  // define the is_street type trait
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
