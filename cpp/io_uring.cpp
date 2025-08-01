#include "io_uring.h"
#include "expected.h"
#include <liburing.h>

IoUring::~IoUring() noexcept { queue_exit(); }

auto IoUring::queue_exit() noexcept -> void {
  if (ring_.ring_fd == invalid_fd)
    return;

  struct io_uring ring = ring_;
  ring_ = {.ring_fd = invalid_fd};
  io_uring_queue_exit(&ring);
}

auto IoUring::queue_init(size_t entries, size_t flags) noexcept
    -> Expected<IoUring> {
  struct io_uring ring {};
  const int ret = io_uring_queue_init(entries, &ring, flags);
  if (ret < 0) {
    return unexpected(std::system_error{make_error_code(-ret),
                                        "Failed to call io_uring_queue_init."});
  }

  return IoUring{ring};
}

IoUring::IoUring(struct io_uring ring) noexcept : ring_{ring} {}
