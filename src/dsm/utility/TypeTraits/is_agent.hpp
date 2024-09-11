
#pragma once

#include <concepts>
#include <memory>
#include <type_traits>
#include "is_numeric.hpp"

namespace dsm {
  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             is_numeric_v<Delay>)
  class Agent;

  // define is_node type trait
  template <typename T>
  struct is_agent : std::false_type {};

  template <typename Id, typename Size, typename Delay>
  struct is_agent<Agent<Id, Size, Delay>> : std::true_type {};

  template <typename Id, typename Size, typename Delay>
  struct is_agent<std::unique_ptr<Agent<Id, Size, Delay>>> : std::true_type {};

  template <typename T>
  inline constexpr bool is_agent_v = is_agent<T>::value;

};  // namespace dsm

