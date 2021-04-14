#include <await/executors/priority.hpp>
#include <await/executors/guarded.hpp>

#include <queue>
#include <memory>

namespace await::executors {

class PriorityTask {
 private:
  Task routine_;
  int priority_;

 public:
  PriorityTask(Task&& routine, int priority)
      : routine_(std::move(routine)), priority_(priority) {
  }

  const Task& GetTask() const {
    return routine_;
  }

  auto operator>(const PriorityTask& other) const {
    return this->priority_ > other.priority_;
  }

  auto operator<(const PriorityTask& other) const {
    return other > *this;
  }

  auto operator==(const PriorityTask& other) const {
    return !(*this > other) && !(other > *this);
  }
};

class FixedPriorityExecutor;

class PriorityExecutor : public IPriorityExecutor {
 private:
  Guarded<std::priority_queue<PriorityTask>> task_queue_;
  IExecutorPtr wrapped_;

 public:
  PriorityExecutor(IExecutorPtr executor) : wrapped_(executor) {
  }

  void Execute(int priority, Task&& task) {
    task_queue_->emplace(std::move(task), priority);

    wrapped_->Execute([this]() {
      Task routine =
          std::move(const_cast<Task&>(task_queue_->top().GetTask()));  // NOLINT
      task_queue_->pop();

      routine();
    });
  }

  IExecutorPtr FixPriority(int priority) {
    return dynamic_pointer_cast<IExecutor, FixedPriorityExecutor>(
        std::make_shared<FixedPriorityExecutor>(this, priority));
  }
};

class FixedPriorityExecutor : public IExecutor {
 private:
  PriorityExecutor* executor_;
  int priority_;

 public:
  FixedPriorityExecutor(PriorityExecutor* executor, int priority)
      : executor_(executor), priority_(priority) {
  }

  void Execute(Task&& task) {
    executor_->Execute(priority_, std::move(task));
  }
};

IPriorityExecutorPtr MakePriorityExecutor(IExecutorPtr executor) {
  return dynamic_pointer_cast<IPriorityExecutor, PriorityExecutor>(
      std::make_shared<PriorityExecutor>(executor));
}

}  // namespace await::executors
