#ifndef is_numeric_hpp
#define is_numeric_hpp

#include <concepts>
#include <memory>
#include <type_traits>

namespace dmf {
  // Alias for shared pointers
  template <typename T>
  using shared = std::shared_ptr<T>;

  // define is_numeric_v type trait
  template <typename T>
  inline constexpr bool is_numeric_v =
      (std::is_integral_v<T> || std::is_floating_point_v<T>)&&!std::is_same_v<T, bool> &&
      !std::is_same_v<T, char>;

};  // namespace dmf

#endif
