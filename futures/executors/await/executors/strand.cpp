#include <await/executors/strand.hpp>
#include <await/executors/helpers.hpp>

#include <twist/stdlike/atomic.hpp>
#include <await/executors/guarded.hpp>

#include <queue>
#include <memory>
#include <mutex>
#include <cstdint>

namespace await::executors {

class Strand : public IExecutor, public std::enable_shared_from_this<Strand> {
 private:
  Guarded<std::queue<Task>> tasks_;  // guarded by mutex_
  IExecutorPtr executor_;
  twist::stdlike::atomic<uint32_t>
      batch_sent_;  // should be kept alive with shared pointer

  Task DequeItem() {
    Task routine = std::move(tasks_->front());
    tasks_->pop();

    return routine;
  }

 public:
  Strand(IExecutorPtr executor) : executor_(executor), batch_sent_(0u) {
  }

  void ExecutorRoutine() {
    executor_->Execute([self = shared_from_this()]() {
      const int batch_size = 50;
      int completed = 0;

      while (completed < batch_size) {
        if (self->tasks_->empty()) {
          self->batch_sent_.store(0);
          return;
        }
        Task routine = self->DequeItem();
        ExecuteHere(routine);
        ++completed;
      }
      self->ExecutorRoutine();
    });
  }

  void Execute(Task&& task) {
    tasks_->push(std::move(task));

    if (batch_sent_.exchange(1) == 0u) {
      ExecutorRoutine();
    }
  }
};

IExecutorPtr MakeStrand(IExecutorPtr executor) {
  return dynamic_pointer_cast<IExecutor, Strand>(
      std::make_shared<Strand>(executor));
}

}  // namespace await::executors
