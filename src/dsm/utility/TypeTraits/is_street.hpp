
#pragma once

#include <concepts>
#include <memory>
#include <type_traits>

namespace dsm {
  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  class Street;

  // define the is_street type trait
  template <typename T>
  struct is_street : std::false_type {};

  template <typename Id, typename Size>
  struct is_street<Street<Id, Size>> : std::true_type {};

  template <typename Id, typename Size>
  struct is_street<const Street<Id, Size>> : std::true_type {};

  template <typename Id, typename Size>
  struct is_street<const Street<Id, Size>&> : std::true_type {};

  template <typename Id, typename Size>
  struct is_street<std::unique_ptr<Street<Id, Size>>> : std::true_type {};

  template <typename T>
  inline constexpr bool is_street_v = is_street<T>::value;

};  // namespace dsm

