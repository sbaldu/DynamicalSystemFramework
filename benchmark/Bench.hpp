// Header containing some basic functions for benchmarking
// Note: In the future the Google benchmark library could be used,
// for now I think that's an overkill

#ifndef Bench_hpp
#define Bench_hpp

#include <chrono>
#include <iostream>
#include <type_traits>

using Average = long double;

template <typename T>
class Timer {
private:
  std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
  T* duration;

public:
  Timer(T* d) : m_start{std::chrono::high_resolution_clock::now()}, duration{d} {}
  ~Timer() {
    auto finish{std::chrono::high_resolution_clock::now()};
    *duration += (finish - m_start).count();
  }
};

template <typename T>
class Bench {
private:
  int m_rep;
  Average m_avgTime;

public:
  Bench() : m_rep{1} {}
  Bench(int rep) : m_rep{rep} {}

  template <typename F, typename... Tn>
    requires std::is_invocable_v<F, Tn...>
  Average benchmark(F func, Tn... args) {
    T average{};

    for (int i{}; i < m_rep; ++i) {
      Timer<T> t(&average);
      func(args...);
    }

    m_avgTime = static_cast<Average>(average) / m_rep;
    return m_avgTime;
  }

  Average time() const { return m_avgTime; }
};

#endif
