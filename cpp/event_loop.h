#pragma once

#include <memory>

#include "fd.h"

struct io_uring;

namespace kero {

class EventLoop {
public:
  struct Config {
    uint io_uring_queue_entries{1024};
  };

  EventLoop() noexcept = default;
  EventLoop(const EventLoop&) = delete;
  EventLoop(EventLoop&&) noexcept = default;

  EventLoop& operator=(const EventLoop&) = delete;
  EventLoop& operator=(EventLoop&&) noexcept = default;

  ~EventLoop() noexcept;

  [[nodiscard]] Result<void, Error> run(const Config& config) noexcept;
  [[nodiscard]] Result<void, Error> stop() noexcept;

private:
  [[nodiscard]] Result<void, Error> open_epoll() noexcept;
  [[nodiscard]] Result<void, Error>
  open_io_uring(uint io_uring_queue_entries) noexcept;

  std::unique_ptr<struct io_uring> ring_;
  Fd epoll_fd_;
  Fd shutdown_event_fd_;
  Fd io_uring_event_fd_;
};

} // namespace kero
