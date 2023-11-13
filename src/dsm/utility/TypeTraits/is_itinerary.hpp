#ifndef is_itinerary_hpp
#define is_itinerary_hpp

#include <concepts>
#include <memory>
#include <type_traits>

namespace dsm {
  template <typename Id>
    requires std::unsigned_integral<Id>
  class Itinerary;

  // Alias for shared pointers
  template <typename T>
  using shared = std::shared_ptr<T>;

  // define is_node type trait
  template <typename T>
  struct is_itinerary : std::false_type {};

  template <typename Id, typename Size, typename Delay>
  struct is_itinerary<Agent<Id, Size, Delay>> : std::true_type {};

  template <typename Id, typename Size, typename Delay>
  struct is_itinerary<shared<Agent<Id, Size, Delay>>> : std::true_type {};

  template <typename T>
  inline constexpr bool is_itinerary_v = is_itinerary<T>::value;

};  // namespace dmf

#endif
