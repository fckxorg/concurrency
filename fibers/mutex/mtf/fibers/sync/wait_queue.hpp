#include <cstddef>

namespace mtf::fibers {
class WaitQueue {
 public:
  WaitQueue() = delete;

  static void Park(void* ptr);
  static size_t Size(void* ptr);
  static void WakeOne(void* ptr);
  static void WakeAll(void* ptr);
};
};  // namespace mtf::fibers
