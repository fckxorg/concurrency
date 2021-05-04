#include <mtf/fibers/core/handle.hpp>

#include <mtf/fibers/core/fiber.hpp>

#include <wheels/support/assert.hpp>

namespace mtf::fibers {

void FiberHandle::Resume() {
  WHEELS_VERIFY(IsValid(), "Invalid fiber handle");
  reinterpret_cast<Fiber*>(fiber_)->Resume();
}

FiberHandle FiberHandle::FromCurrent() {
  Fiber& fiber = Fiber::AccessCurrent();
  return FiberHandle(&fiber);
}

void FiberHandle::Suspend() {
  WHEELS_VERIFY(IsValid(), "Invalid fiber handle");
  reinterpret_cast<Fiber*>(fiber_)->Suspend();
}

}  // namespace mtf::fibers
