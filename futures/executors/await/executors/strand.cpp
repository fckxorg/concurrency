#include <await/executors/strand.hpp>
#include <await/executors/helpers.hpp>

#include <twist/stdlike/atomic.hpp>

#include <queue>
#include <memory>
#include <mutex>

namespace await::executors {

class Strand : public IExecutor {
 private:
  std::shared_ptr<std::queue<Task>> tasks_;  // guarded by mutex_
  IExecutorPtr executor_;
  std::mutex mutex_;
  twist::stdlike::atomic<uint32_t> batch_sent_;

 public:
  Strand(IExecutorPtr executor) : executor_(executor), batch_sent_(false) {
    tasks_ = std::make_shared<std::queue<Task>>();
  }

  void ExecutorRoutine() {
    auto task_queue_ = std::shared_ptr(tasks_);

    executor_->Execute([this, task_queue_]() {
      int completed = 0;

      while (completed < 50) {
        Task routine;
        {
          std::lock_guard lock(mutex_);
          if (task_queue_->empty()) {
            batch_sent_.store(0);
            break;
          }
          routine = std::move(task_queue_->front());
          task_queue_->pop();
        }

        ExecuteHere(routine);
        ++completed;
      }
      if (batch_sent_.load()) {
        ExecutorRoutine();
      }
    });
  }

  void Execute(Task&& task) {
    {
      std::lock_guard lock(mutex_);
      tasks_->push(std::move(task));
    }

    if (!batch_sent_.load()) {
      batch_sent_.store(1);

      if (!tasks_->empty()) {
        ExecutorRoutine();
      }
    }
  }
};

IExecutorPtr MakeStrand(IExecutorPtr executor) {
  return dynamic_pointer_cast<IExecutor, Strand>(
      std::make_shared<Strand>(executor));
}

}  // namespace await::executors
