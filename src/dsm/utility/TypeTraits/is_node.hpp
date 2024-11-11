
#pragma once

#include <concepts>
#include <memory>
#include <type_traits>

namespace dsm {
  class Node;

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  class Intersection;

  template <typename Id, typename Size, typename Delay>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size> &&
             std::unsigned_integral<Delay>)
  class TrafficLight;

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  class Roundabout;

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

  // TODO: this is bad, I'll rework the type-traits
  template <typename Id, typename Size>
  struct is_node<Intersection<Id, Size>> : std::true_type {};

  template <typename Id, typename Size>
  struct is_node<const Intersection<Id, Size>> : std::true_type {};

  template <typename Id, typename Size>
  struct is_node<const Intersection<Id, Size>&> : std::true_type {};

  template <typename Id, typename Size>
  struct is_node<std::unique_ptr<Intersection<Id, Size>>> : std::true_type {};

  template <typename Id, typename Size, typename Delay>
  struct is_node<TrafficLight<Id, Size, Delay>> : std::true_type {};

  template <typename Id, typename Size, typename Delay>
  struct is_node<const TrafficLight<Id, Size, Delay>> : std::true_type {};

  template <typename Id, typename Size, typename Delay>
  struct is_node<const TrafficLight<Id, Size, Delay>&> : std::true_type {};

  template <typename Id, typename Size, typename Delay>
  struct is_node<std::unique_ptr<TrafficLight<Id, Size, Delay>>> : std::true_type {};

  template <typename Id, typename Size>
  struct is_node<Roundabout<Id, Size>> : std::true_type {};

  template <typename Id, typename Size>
  struct is_node<const Roundabout<Id, Size>> : std::true_type {};

  template <typename Id, typename Size>
  struct is_node<const Roundabout<Id, Size>&> : std::true_type {};

  template <typename Id, typename Size>
  struct is_node<std::unique_ptr<Roundabout<Id, Size>>> : std::true_type {};

  template <typename T>
  inline constexpr bool is_node_v = is_node<T>::value;

};  // namespace dsm
