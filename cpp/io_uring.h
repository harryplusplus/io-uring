#pragma once

#include <liburing.h>

#include "fd.h"
#include "result.h"

class EventFd;

class IoUring {
public:
  IoUring(const IoUring&) = delete;
  IoUring(IoUring&& other) noexcept;

  ~IoUring() noexcept;

  auto operator=(const IoUring&) -> IoUring& = delete;
  auto operator=(IoUring&& other) noexcept -> IoUring&;

  auto queue_exit() noexcept -> void;

  auto register_eventfd_async(const EventFd& event_fd) noexcept -> Result<void>;
  auto unregister_eventfd() noexcept -> Result<void>;

  [[nodiscard]] static auto queue_init(unsigned int entries,
                                       unsigned int flags) noexcept
      -> Result<IoUring>;

private:
  IoUring(struct io_uring ring) noexcept;

  constexpr static auto create_invalid_ring() noexcept -> struct io_uring {
    return {.ring_fd = Fd::invalid_raw_fd};
  }

  constexpr static auto
  is_invalid_ring(const struct io_uring& ring) noexcept -> bool {
    return ring.ring_fd == Fd::invalid_raw_fd;
  }

  struct io_uring ring_ {
    create_invalid_ring()
  };
};
