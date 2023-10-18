
#ifndef Graph_hpp
#define Graph_hpp

#include <concepts>
#include <memory>
#include <unordered_set>
#include <queue>
#include <type_traits>

#include "Node.hpp"
#include "SparseMatrix.hpp"
#include "Street.hpp"
#include "utility/HashFunctions.hpp"
#include "utility/TypeTraits.hpp"

namespace dmf {

  // Alias for shared pointers
  template <typename T>
  using shared = std::shared_ptr<T>;
  using std::make_shared;

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  class Graph {
  private:
    shared<SparseMatrix<Id, bool>> m_adjacency;
    std::unordered_set<shared<Node<Id>>, nodeHash<Id>> m_nodes;
    std::unordered_set<shared<Street<Id, Size>>, streetHash<Id, Size>> m_streets;

  public:
    Graph();
    Graph(const std::unordered_set<shared<Street<Id, Size>>, nodeHash<Id>>& streetSet);

    void buildAdj();

    void addNode(shared<Node<Id>> node);
    void addNode(const Node<Id>& node);

    template <typename... Tn>
      requires(is_node_v<Tn> && ...)
    void addNodes(Tn... nodes);

    template <typename T1, typename... Tn>
      requires is_node_v<T1> && (is_node_v<Tn> && ...)
    void addNodes(T1 node, Tn... nodes);

    void addStreet(shared<Street<Id, Size>> street);
    void addStreet(const Street<Id, Size>& street);

    template <typename... Tn>
      requires(is_street_v<Tn> && ...)
    void addStreets(Tn... streets);

    template <typename T1, typename... Tn>
      requires is_street_v<T1> && (is_street_v<Tn> && ...)
    void addStreets(T1 street, Tn... streets);

    // getters
    shared<SparseMatrix<Id, bool>> adjMatrix() const;
    std::unordered_set<shared<Node<Id>>, nodeHash<Id>> nodeSet() const;
    std::unordered_set<shared<Street<Id, Size>>, streetHash<Id, Size>> streetSet() const;
  };

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  Graph<Id, Size>::Graph()
      : m_adjacency{make_shared<SparseMatrix<Id, bool>>()} {}

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  Graph<Id, Size>::Graph(const std::unordered_set<shared<Street<Id, Size>>, nodeHash<Id>>& streetSet)
      : m_adjacency{make_shared<SparseMatrix<Id, bool>>()} {
    for (auto street : streetSet) {
      m_streets.insert(street);
      m_nodes.insert(street->nodes().first);
      m_nodes.insert(street->nodes().second);
    }

    buildAdj();
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Graph<Id, Size>::buildAdj() {
    for (auto street : m_streets) {
      m_adjacency->insert(street->nodes().first, street->nodes().second, true);
    }
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Graph<Id, Size>::addNode(shared<Node<Id>> node) {
    m_nodes.insert(node);
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Graph<Id, Size>::addNode(const Node<Id>& node) {
    m_nodes.insert(make_shared<Node<Id>>(node));
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  template <typename... Tn>
    requires(is_node_v<Tn> && ...)
  void Graph<Id, Size>::addNodes(Tn... nodes) {}

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  template <typename T1, typename... Tn>
    requires is_node_v<T1> && (is_node_v<Tn> && ...)
  void Graph<Id, Size>::addNodes(T1 node, Tn... nodes) {
    addNode(node);
    addNodes(nodes...);
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Graph<Id, Size>::addStreet(shared<Street<Id, Size>> street) {
    m_streets.insert(street);
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  void Graph<Id, Size>::addStreet(const Street<Id, Size>& street) {
    m_streets.insert(make_shared<Street<Id, Size>>(street));
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  template <typename... Tn>
    requires(is_street_v<Tn> && ...)
  void Graph<Id, Size>::addStreets(Tn... edges) {}

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  template <typename T1, typename... Tn>
    requires is_street_v<T1> && (is_street_v<Tn> && ...)
  void Graph<Id, Size>::addStreets(T1 street, Tn... streets) {
    addStreet(street);
    addStreets(streets...);
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  shared<SparseMatrix<Id, bool>> Graph<Id, Size>::adjMatrix() const {
    return m_adjacency;
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  std::unordered_set<shared<Node<Id>>, nodeHash<Id>> Graph<Id, Size>::nodeSet() const {
    return m_nodes;
  }

  template <typename Id, typename Size>
    requires(std::unsigned_integral<Id> && std::unsigned_integral<Size>)
  std::unordered_set<shared<Street<Id, Size>>, streetHash<Id, Size>> Graph<Id, Size>::streetSet() const {
    return m_streets;
  }
};  // namespace dmf

#endif
