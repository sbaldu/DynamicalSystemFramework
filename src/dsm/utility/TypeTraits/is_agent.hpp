
#pragma once

#include <concepts>
#include <memory>
#include <type_traits>
#include "is_numeric.hpp"

namespace dsm {
  template <typename Delay>
    requires(is_numeric_v<Delay>)
  class Agent;

  // define is_node type trait
  template <typename T>
  struct is_agent : std::false_type {};

  template <typename Delay>
  struct is_agent<Agent<Delay>> : std::true_type {};

  template <typename Delay>
  struct is_agent<std::unique_ptr<Agent<Delay>>> : std::true_type {};

  template <typename T>
  inline constexpr bool is_agent_v = is_agent<T>::value;

};  // namespace dsm
