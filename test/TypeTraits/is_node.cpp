#include <cassert>
#include <cstdint>
#include <memory>

#include "is_node.hpp"

using dmf::is_node;
using dmf::is_node_v;
using dmf::Node;

// check the type trait
static_assert(is_node<Node<uint8_t>>::value);
static_assert(is_node<Node<uint16_t>>::value);
static_assert(is_node<Node<uint32_t>>::value);
static_assert(!is_node<int>::value);
static_assert(!is_node<unsigned int>::value);
static_assert(!is_node<std::string>::value);
static_assert(is_node<std::shared_ptr<Node<uint8_t>>>::value);
static_assert(is_node<std::shared_ptr<Node<uint16_t>>>::value);
static_assert(is_node<std::shared_ptr<Node<uint32_t>>>::value);
static_assert(!is_node<std::shared_ptr<int>>::value);
static_assert(!is_node<std::shared_ptr<unsigned int>>::value);
static_assert(!is_node<std::shared_ptr<std::string>>::value);

// check the template variable
static_assert(is_node_v<Node<uint8_t>>);
static_assert(is_node_v<Node<uint16_t>>);
static_assert(is_node_v<Node<uint32_t>>);
static_assert(!is_node_v<int>);
static_assert(!is_node_v<unsigned int>);
static_assert(!is_node_v<std::string>);
static_assert(is_node_v<std::shared_ptr<Node<uint8_t>>>);
static_assert(is_node_v<std::shared_ptr<Node<uint16_t>>>);
static_assert(is_node_v<std::shared_ptr<Node<uint32_t>>>);
static_assert(!is_node_v<std::shared_ptr<int>>);
static_assert(!is_node_v<std::shared_ptr<unsigned int>>);
static_assert(!is_node_v<std::shared_ptr<std::string>>);

int main() {}
