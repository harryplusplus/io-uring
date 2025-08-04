#pragma once

#include <functional>
#include <memory>
#include <vector>

#include "error.h"
#include "result.h"

struct io_uring;

namespace kero {

using Fd = int;
static constexpr Fd invalid_fd{-1};

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
  [[nodiscard]] Result<void, Error>
  open_io_uring(uint io_uring_queue_entries) noexcept;
  [[nodiscard]] Result<void, Error> open_epoll() noexcept;
  [[nodiscard]] Result<Fd, Error> open_event_fd() const noexcept;
  [[nodiscard]] Result<void, Error> open_io_uring_event_fd() noexcept;

  std::vector<std::function<void()>> closers_;
  std::unique_ptr<struct io_uring> ring_;
  Fd epoll_fd_{invalid_fd};
  Fd shutdown_event_fd_{invalid_fd};
  Fd io_uring_event_fd_{invalid_fd};
};

} // namespace kero
