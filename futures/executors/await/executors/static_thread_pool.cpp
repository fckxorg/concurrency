#include <await/executors/static_thread_pool.hpp>

#include <await/executors/label_thread.hpp>
#include <await/executors/helpers.hpp>

#include <twist/util/thread_local.hpp>
#include <memory>

namespace await::executors {
////////////////////////////////////////////////////////////////////////////////

static twist::util::ThreadLocal<StaticThreadPool*> pool{nullptr};

////////////////////////////////////////////////////////////////////////////////

StaticThreadPool::StaticThreadPool(size_t n_workers, const std::string& label)
    : runners_(0), executed_task_count_(0), finished_(false) {
  no_tasks_left_.store(0);
  for (size_t i = 0; i < n_workers; ++i) {
    workers_.emplace_back([this, &label]() {
      LabelThread(label);
      WorkerRoutine();
    });
  }
}

StaticThreadPool::~StaticThreadPool() {
  assert(finished_);
}

void StaticThreadPool::Execute(Task&& new_task) {
  runners_.fetch_add(1);
  queue_.Put(std::move(new_task));
}

void StaticThreadPool::Join() {
  while (runners_ != 0) {
    no_tasks_left_.wait(0);
  }
  queue_.Close();
  JoinWorkers();
  finished_ = true;
}

void StaticThreadPool::Shutdown() {
  queue_.Cancel();
  JoinWorkers();
  finished_ = true;
}

size_t StaticThreadPool::ExecutedTaskCount() const {
  return executed_task_count_.load();
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
    executed_task_count_.fetch_add(1);
    if (runners_.fetch_sub(1) == 1) {
      no_tasks_left_.store(1);
      no_tasks_left_.notify_one();
    }
  }
}

IThreadPoolPtr MakeStaticThreadPool(size_t threads, const std::string& name) {
  return dynamic_pointer_cast<IThreadPool, StaticThreadPool>(
      std::make_shared<StaticThreadPool>(threads, name));
}

}  // namespace await::executors
