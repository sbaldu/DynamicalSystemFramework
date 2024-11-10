
#pragma once

#include <concepts>
#include <memory>
#include <type_traits>

namespace dsm {
  class Node;

  // define is_node type trait
  template <typename T>
  struct is_node : std::false_type {};

  template <>
  struct is_node<Node> : std::true_type {};

  template <>
  struct is_node<const Node> : std::true_type {};

  template <>
  struct is_node<const Node&> : std::true_type {};

  template <>
  struct is_node<std::unique_ptr<Node>> : std::true_type {};

  template <typename T>
  inline constexpr bool is_node_v = is_node<T>::value;

};  // namespace dsm
