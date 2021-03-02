#include <cstdio>
#include <tp/static_thread_pool.hpp>

#include <tp/helpers.hpp>

#include <twist/util/thread_local.hpp>

namespace tp {

////////////////////////////////////////////////////////////////////////////////

static twist::util::ThreadLocal<StaticThreadPool*> pool{nullptr};

////////////////////////////////////////////////////////////////////////////////

StaticThreadPool::StaticThreadPool(size_t n_workers) : runners_(0) {
  cv_.store(0);
  for (size_t i = 0; i < n_workers; ++i) {
    workers_.emplace_back([this]() {
      WorkerRoutine();
    });
  }
}

StaticThreadPool::~StaticThreadPool() {
}

void StaticThreadPool::Submit(Task new_task) {
  runners_.fetch_add(1);
  queue_.Put(std::move(new_task));
}

void StaticThreadPool::Join() {
  while (runners_ != 0) {
    cv_.wait(0);
  }
  queue_.Close();
  JoinWorkers();
}

void StaticThreadPool::Shutdown() {
  queue_.Cancel();
  JoinWorkers();
}

StaticThreadPool* StaticThreadPool::Current() {
  return *pool;
}

void StaticThreadPool::JoinWorkers() {
  for (auto& worker : workers_) {
    worker.join();
  }
}

void StaticThreadPool::WorkerRoutine() {
  *pool = this;
  while (true) {
    auto task = queue_.Take();

    if (!task.has_value()) {
      break;
    }

    ExecuteHere(*task);
    if (runners_.fetch_sub(1) == 1) {
      cv_.store(1);
      cv_.notify_one();
    }
  }
}

}  // namespace tp
