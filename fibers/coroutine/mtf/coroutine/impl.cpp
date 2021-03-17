#include <mtf/coroutine/impl.hpp>

namespace mtf::coroutine::impl {

thread_local Coroutine* Coroutine::current_coroutine{nullptr};

Coroutine::Coroutine(Routine routine, context::StackView stack)
    : routine_(std::move(routine)),
      is_completed_(false),
      routine_exception_(nullptr) {
  coroutine_context_.Setup(stack, Coroutine::Trampoline);
}

void Coroutine::Resume() {
  Coroutine* prev_coroutine = std::exchange(current_coroutine, this);

  caller_context_.SwitchTo(coroutine_context_);
  current_coroutine = prev_coroutine;

  if (routine_exception_) {
    std::rethrow_exception(routine_exception_);
  }
}

void Coroutine::Suspend() {
  Coroutine* suspended = current_coroutine;
  suspended->coroutine_context_.SwitchTo(suspended->caller_context_);
}

bool Coroutine::IsCompleted() const {
  return is_completed_;
}

void Coroutine::Trampoline() {
  current_coroutine->coroutine_context_.AfterStart();

  try {
    current_coroutine->routine_();
  } catch (...) {
    current_coroutine->routine_exception_ = std::current_exception();
  }

  current_coroutine->is_completed_ = true;

  current_coroutine->coroutine_context_.SwitchTo(
      current_coroutine->caller_context_);

  std::abort();
}

}  // namespace mtf::coroutine::impl
