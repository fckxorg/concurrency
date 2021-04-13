#pragma once

#include <vector>
#include <cstdint>

#include <await/executors/thread_pool.hpp>
#include <twist/stdlike/atomic.hpp>
#include <twist/stdlike/thread.hpp>
#include <await/executors/blocking_queue.hpp>

namespace await::executors {

class StaticThreadPool : public IThreadPool {
 public:
  explicit StaticThreadPool(size_t workers, const std::string& label);
  ~StaticThreadPool();

  // Non-copyable
  StaticThreadPool(const StaticThreadPool&) = delete;
  StaticThreadPool& operator=(const StaticThreadPool&) = delete;

  // Schedules task for execution in one of the worker threads
  void Execute(Task&& task);

  // Graceful shutdown
  // Waits until outstanding work count has reached 0
  // and joins worker threads
  void Join();

  // Hard shutdown
  // Joins worker threads ASAP
  void Shutdown();

  size_t ExecutedTaskCount() const;

  // Locate current thread pool from worker thread
  static StaticThreadPool* Current();

 private:
  UnboundedBlockingQueue<Task> queue_;
  std::vector<twist::stdlike::thread> workers_;
  twist::stdlike::atomic<uint32_t> runners_;
  twist::stdlike::atomic<uint32_t> no_tasks_left_;
  twist::stdlike::atomic<uint32_t> executed_task_count_;

  bool finished_;

  void JoinWorkers();

  void WorkerRoutine();
};

inline StaticThreadPool* Current() {
  return StaticThreadPool::Current();
}

// Fixed-size pool of threads + unbounded blocking queue
IThreadPoolPtr MakeStaticThreadPool(size_t threads, const std::string& name);

}  // namespace await::executors
