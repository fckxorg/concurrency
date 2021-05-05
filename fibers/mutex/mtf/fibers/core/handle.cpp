#include <mtf/fibers/core/handle.hpp>

#include <mtf/fibers/core/fiber.hpp>

#include <wheels/support/assert.hpp>

namespace mtf::fibers {

void FiberHandle::Resume() {
  WHEELS_VERIFY(IsValid(), "Invalid fiber handle");
  reinterpret_cast<Fiber*>(fiber_)->Resume();
}

void FiberHandle::Suspend(Awaiter* awaiter) {
  reinterpret_cast<Fiber*>(fiber_)->Suspend(awaiter);
}

}  // namespace mtf::fibers
