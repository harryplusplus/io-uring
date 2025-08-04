#pragma once

#include <memory>

#include "fd.h"

struct io_uring;

namespace kero {

class EventLoop {
public:
  EventLoop() noexcept = default;
  EventLoop(const EventLoop&) = delete;
  EventLoop(EventLoop&&) noexcept = default;

  EventLoop& operator=(const EventLoop&) = delete;
  EventLoop& operator=(EventLoop&&) noexcept = default;

  ~EventLoop() noexcept;

  Result<void, Error> run() noexcept;

private:
  std::unique_ptr<struct io_uring> ring_;
  Fd epoll_fd_;
  Fd shutdown_event_fd_;
  Fd io_uring_event_fd_;
};

} // namespace kero
