#pragma once

#include "../utility/Typedef.hpp"

#include <atomic>
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
    std::atomic<size_t> m_nActiveTasks;
    std::vector<std::thread> m_threads;
    std::queue<std::function<void()>> m_tasks;
    std::mutex m_mutex;
    std::condition_variable m_cv;

  public:
    ThreadPool(const unsigned int nThreads = std::thread::hardware_concurrency());
    ~ThreadPool();

    void enqueue(std::function<void()> task);
    void waitAll();
  };
}  // namespace dsm