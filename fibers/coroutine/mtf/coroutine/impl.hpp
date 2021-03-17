#pragma once

#include <mtf/coroutine/routine.hpp>

#include <context/context.hpp>

#include <exception>

namespace mtf::coroutine::impl {

// Stackful asymmetric coroutine

class Coroutine {
 public:
  Coroutine(Routine routine, context::StackView stack);

  // Non-copyable
  Coroutine(const Coroutine&) = delete;
  Coroutine& operator=(const Coroutine&) = delete;
  Coroutine(Coroutine&& other) = default;

  void Resume();

  // Suspends current coroutine
  static void Suspend();

  bool IsCompleted() const;

  Routine routine_;

 private:
  [[noreturn]] static void Trampoline();

  context::ExecutionContext coroutine_context_;
  context::ExecutionContext caller_context_;

  bool is_completed_;

  static thread_local Coroutine* current_coroutine;

  std::exception_ptr routine_exception_;
};

}  // namespace mtf::coroutine::impl
