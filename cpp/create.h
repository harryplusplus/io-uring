#pragma once

#include "error.h"
#include "result.h"
#include "runner.h"
#include "stopper.h"

namespace kero {

struct CreateInput {
  uint io_uring_queue_entries = 1024;
};

struct CreateOutput {
  Runner runner;
  Stopper stopper;
};

[[nodiscard]] Result<CreateOutput, Error> create(CreateInput&& input) noexcept;

} // namespace kero
