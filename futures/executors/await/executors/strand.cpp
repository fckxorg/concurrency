#include <await/executors/strand.hpp>
#include <await/executors/helpers.hpp>

#include <twist/stdlike/atomic.hpp>

#include <queue>
#include <memory>
#include <mutex>

namespace await::executors {

class Strand : public IExecutor {
 private:
  using QueuePtr = std::shared_ptr<std::queue<Task>>;

  QueuePtr tasks_;  // guarded by mutex_
  IExecutorPtr executor_;
  std::mutex mutex_;
  std::shared_ptr<int> batch_sent_;  // should be kept alive with shared pointer

  Task dequeItem(QueuePtr task_queue) {
    std::lock_guard lock(mutex_);
    Task routine = std::move(task_queue->front());
    task_queue->pop();

    return routine;
  }

 public:
  Strand(IExecutorPtr executor) : executor_(executor), batch_sent_(false) {
    tasks_ = std::make_shared<std::queue<Task>>();
    batch_sent_ = std::make_shared<int>(0);
  }

  void ExecutorRoutine(QueuePtr alive_queue, std::shared_ptr<int> batch_flag) {
    executor_->Execute([this, alive_queue, batch_flag]() {
      int completed = 0;

      while (completed < 50) {
        {
          std::lock_guard lock(mutex_);
          if (alive_queue->empty()) {
            *batch_flag = 0;
            return;
          }
        }
        Task routine = dequeItem(alive_queue);
        ExecuteHere(routine);
        ++completed;
      }
      ExecutorRoutine(alive_queue, batch_flag);
    });
  }

  void Execute(Task&& task) {
    std::lock_guard lock(mutex_);
    tasks_->push(std::move(task));

    if (!batch_sent_.load()) {
      *batch_sent_ = 1;

      if (!tasks_->empty()) {
        ExecutorRoutine(tasks_, batch_sent_);
      }
    }
  }
};

IExecutorPtr MakeStrand(IExecutorPtr executor) {
  return dynamic_pointer_cast<IExecutor, Strand>(
      std::make_shared<Strand>(executor));
}

}  // namespace await::executors
