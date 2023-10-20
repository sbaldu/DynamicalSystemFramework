#ifndef is_numeric_hpp
#define is_numeric_hpp

#include <concepts>
#include <cstdint>
#include <memory>
#include <type_traits>

namespace dmf {
  // Alias for shared pointers
  template <typename T>
  using shared = std::shared_ptr<T>;

  // define is_numeric_v type trait
  template <typename T>
  struct is_numeric : std::is_arithmetic<T>{};

  template <>
  struct is_numeric<bool> : std::false_type{};
  template <>
  struct is_numeric<char> : std::false_type{};

  template <typename T>
  inline constexpr bool is_numeric_v = is_numeric<T>::value;

};  // namespace dmf

#endif
