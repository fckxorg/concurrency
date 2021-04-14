#include <await/executors/strand.hpp>
#include <await/executors/helpers.hpp>
#include <await/executors/guarded.hpp>
#include <queue>

namespace await::executors {

class Strand : public IExecutor {
 private:
  Guarded<std::queue<Task>> tasks_;
  IExecutorPtr executor_;

 public:
  Strand(IExecutorPtr executor) : executor_(executor) {
  }

  void Execute(Task&& task) {
    tasks_.push(std::move(task));

    executor_->Execute([this]() {
      Task routine = std::move(tasks_->front());
      tasks_->pop();
      routine();
    });
  }

}

IExecutorPtr
MakeStrand(IExecutorPtr /*executor*/) {
  return nullptr;  // Not implemented
}

}  // namespace await::executors
