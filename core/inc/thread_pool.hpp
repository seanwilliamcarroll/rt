#pragma once

#include <algorithm>
#include <functional>
#include <iostream>
#include <list>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
public:
  using t_JobFunction = std::function<void(void)>;

  ThreadPool(const unsigned int num_threads)
      : m_num_threads(
            std::min<unsigned int>(std::thread::hardware_concurrency() - 1,
                                   std::max<unsigned int>(num_threads, 1)))
      , m_should_terminate(false)
      , m_jobs_added(0)
      , m_jobs_finished(0) {
    std::clog << "Hardware concurrency: " << std::thread::hardware_concurrency()
              << " threads" << std::endl;
    std::clog << "Using: " << m_num_threads << " threads" << std::endl;
    m_threads.reserve(m_num_threads);
  }

  ThreadPool()
      : ThreadPool(std::thread::hardware_concurrency()) {}

  void start() {
    for (unsigned int thread_index = 0; thread_index < m_num_threads;
         ++thread_index) {
      // Create threads
      m_threads.emplace_back(&ThreadPool::thread_loop, this);
    }
  }

  void add_job(const t_JobFunction &job) {
    {
      std::unique_lock<std::mutex> lock(m_job_queue_mutex);
      m_job_queue.push(job);
      ++m_jobs_added;
    }
    m_job_queue_mutex_condition.notify_one();
  }

  void wait_for_empty_job_queue() {
    {
      std::unique_lock<std::mutex> lock(m_job_queue_mutex);
      m_job_queue_empty_condition.wait(lock,
                                       [this] { return m_job_queue.empty(); });
    }
  }

  bool is_busy() {
    bool is_pool_busy;
    {
      std::unique_lock<std::mutex> lock(m_job_queue_mutex);
      is_pool_busy = !m_job_queue.empty();
    }
    return is_pool_busy;
  }

  void stop() {
    {
      std::unique_lock<std::mutex> lock(m_job_queue_mutex);
      m_should_terminate = true;
    }
    m_job_queue_mutex_condition.notify_all();
    for (auto &active_thread : m_threads) {
      active_thread.join();
    }
    m_threads.clear();
  }

private:
  void thread_loop() {
    for (;;) {
      t_JobFunction job;
      {
        std::unique_lock<std::mutex> lock(m_job_queue_mutex);
        m_job_queue_mutex_condition.wait(lock, [this] {
          return !m_job_queue.empty() || m_should_terminate;
        });
        if (m_should_terminate) {
          return;
        }
        job = m_job_queue.front();
        m_job_queue.pop();
        if (m_job_queue.empty()) {
          m_job_queue_empty_condition.notify_one();
        }
      }
      job();
      {
        std::unique_lock<std::mutex> lock(m_jobs_finished_mutex);
        ++m_jobs_finished;
        std::clog << "\rJobs finished: " << m_jobs_finished << " / "
                  << m_jobs_added << "          " << std::flush;
      }
    }
  }

private:
  const unsigned int m_num_threads;
  bool m_should_terminate;
  std::mutex m_job_queue_mutex;
  std::condition_variable m_job_queue_mutex_condition;
  std::condition_variable m_job_queue_empty_condition;
  std::vector<std::thread> m_threads;
  std::queue<t_JobFunction, std::list<t_JobFunction>> m_job_queue;
  unsigned int m_jobs_added;
  unsigned int m_jobs_finished;
  std::mutex m_jobs_finished_mutex;
};
