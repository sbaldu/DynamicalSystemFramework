
#include <cstdint>
#include <iostream>
#include <random>
#include "Bench.hpp"

#include "Graph.hpp"

using Agent = dmf::Agent<uint32_t>;
using Node = dmf::Node<uint32_t>;
using Street = dmf::Street<uint32_t, uint32_t>;
using SparseMatrix = dmf::SparseMatrix<uint32_t, bool>;

using Bench = sb::Bench<long long int>;

int main() {
  Street street(0, 1000, 10.);
  Agent agent(0, 0);
  Bench b(1000);

  std::cout << "Benchmarking enqueue\n";
  b.benchmark([&street](Agent ag) -> void { street.enqueue(ag); }, agent);
  b.print();

  std::cout << "Benchmarking dequeue\n";
  b.benchmark([&street]() -> void { street.dequeue(); });
  b.print();
}
