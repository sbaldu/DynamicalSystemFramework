#ifndef HashFunctions_hpp
#define HashFunctions_hpp

#include <functional>
#include <memory>

#include "../headers/Node.hpp"
#include "../headers/Street.hpp"

namespace dmf {

  // Alias for shared pointers
  template <typename T>
  using shared = std::shared_ptr<T>;

  template <typename Id>
  struct nodeHash {
    // pointers to const nodes
    size_t operator()(const shared<const Node<Id>>& node) const { return node->id(); }
    // pointers to non-const nodes
    size_t operator()(const shared<Node<Id>>& node) const { return node->id(); }
  };

  template <typename Id, typename Size>
  struct streetHash {
    // pointers to const streets
    size_t operator()(const shared<const Street<Id, Size>>& street) const {
      return (street->nodePair().first) ^ (street->getNodePair().second);
    }
    // pointers to non-const streets
    size_t operator()(const shared<Street<Id, Size>>& street) const {
      return (street->nodePair().first) ^ (street->nodePair().second);
    }
  };

  // Equality operators for Nodes
  template <typename Id>
  bool operator==(shared<const Node<Id>> p1, shared<const Node<Id>> p2) {
    return p1->id() == p2->id();
  }
  template <typename Id>
  bool operator==(shared<Node<Id>> p1, shared<Node<Id>> p2) {
    return p1->id() == p2->id();
  }

  // Equality operators for Streets
  template <typename Id, typename Size>
  bool operator==(shared<const Street<Id, Size>> p1, shared<const Street<Id, Size>> p2) {
    return p1->nodePair().first == p2->nodePair().first && p1->nodePair().second == p2->nodePair().second;
  }
};  // namespace dmf

#endif
