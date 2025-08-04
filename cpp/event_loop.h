#pragma once

#include <memory>

#include "error.h"
#include "result.h"

struct io_uring;

namespace kero {

class EventLoop {
public:
  using Fd = int;

  EventLoop() noexcept = default;
  EventLoop(const EventLoop&) = delete;
  EventLoop(EventLoop&& other) noexcept;

  EventLoop& operator=(const EventLoop&) = delete;
  EventLoop& operator=(EventLoop&& other) noexcept;

  ~EventLoop() noexcept;

  Result<void, Error> run() noexcept;

  static constexpr Fd invalid_fd = -1;

private:
  std::unique_ptr<struct io_uring> ring_{};
  Fd epoll_fd_{invalid_fd};
  Fd shutdown_event_fd_{invalid_fd};
  Fd io_uring_event_fd_{invalid_fd};
};

} // namespace kero
