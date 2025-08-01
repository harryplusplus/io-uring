#pragma once

#include "expected.h"
#include <liburing.h>

class IoUring {
public:
  ~IoUring() noexcept;

  auto queue_exit() noexcept -> void;

  [[nodiscard]] static auto queue_init(size_t entries, size_t flags) noexcept
      -> Expected<IoUring>;

private:
  explicit IoUring(struct io_uring ring) noexcept;

  static constexpr int invalid_fd = -1;
  struct io_uring ring_ {
    .ring_fd = invalid_fd
  };
};
