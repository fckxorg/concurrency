#pragma once
#include <mtf/fibers/core/handle.hpp>

namespace mtf::fibers {

// We need better API
void Suspend(Awaiter* awaiter);

}  // namespace mtf::fibers
