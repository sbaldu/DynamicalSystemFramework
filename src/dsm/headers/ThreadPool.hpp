#pragma once

#include "../utility/Typedef.hpp"

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace dsm {
  class ThreadPool {
  private:
    bool m_stop;
    std::vector<std::thread> m_threads;
    std::queue<std::function<void()>> m_tasks;
    std::mutex m_mutex;
    std::condition_variable m_cv;

  public:
    ThreadPool(unsigned int nThreads = std::thread::hardware_concurrency());
    ~ThreadPool();

    void enqueue(std::function<void()> task);
  };
}  // namespace dsm