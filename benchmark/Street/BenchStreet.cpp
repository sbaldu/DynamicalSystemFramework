
#include <cstdint>
#include <iostream>
#include <random>
#include "Bench.hpp"

#include "Graph.hpp"

using Agent = dsm::Agent<uint32_t, uint32_t, double>;
using Node = dsm::Node<uint32_t, uint32_t>;
using Street = dsm::Street<uint32_t, uint32_t>;
using SparseMatrix = dsm::SparseMatrix<uint32_t, bool>;

using Bench = sb::Bench<long long int>;

int main() {
  Street street(0, 1000, 10., std::make_pair(0, 1));
  Agent agent(0, 0, 0);
  Bench b(1000);

  std::cout << "Benchmarking enqueue\n";
  b.benchmark([&street](Agent ag) -> void { street.enqueue(ag.id()); }, agent);
  b.print();

  std::cout << "Benchmarking dequeue\n";
  b.benchmark([&street]() -> void { street.dequeue(); });
  b.print();
}
