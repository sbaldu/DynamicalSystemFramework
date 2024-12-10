#include "ThreadPool.hpp"

#include <algorithm>
#include <stdexcept>

namespace dsm {
  ThreadPool::ThreadPool(const unsigned int nThreads) : m_stop(false) {
    for (size_t i = 0; i < nThreads; ++i) {
      m_threads.emplace_back([this]() {
        while (true) {
          std::function<void()> task;
          {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_cv.wait(lock, [this]() { return m_stop || !m_tasks.empty(); });

            if (m_stop && m_tasks.empty()) {
              return;
            }

            task = std::move(m_tasks.front());
            m_tasks.pop();
          }
          task();
        }
      });
    }
  }

  ThreadPool::~ThreadPool() {
    {
      std::unique_lock<std::mutex> lock(m_mutex);
      m_stop = true;
      m_cv.notify_all();
    }
    for (auto& thread : m_threads) {
      thread.join();
    }
  }

  void ThreadPool::enqueue(std::function<void()> task) {
    {
      std::unique_lock<std::mutex> lock(m_mutex);
      m_tasks.emplace(std::move(task));
    }
    m_cv.notify_one();
  }
}  // namespace dsm