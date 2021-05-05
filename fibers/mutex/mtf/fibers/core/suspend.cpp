#include <mtf/fibers/core/suspend.hpp>

#include <mtf/fibers/core/fiber.hpp>

namespace mtf::fibers {

void Suspend(Awaiter* awaiter) {
  Fiber::AccessCurrent().Suspend(awaiter);
}

}  // namespace mtf::fibers
