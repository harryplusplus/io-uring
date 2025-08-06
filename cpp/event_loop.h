#ifndef KERO_EVENT_LOOP_H_
#define KERO_EVENT_LOOP_H_

#include <atomic>
#include <memory>

#include "config.h"
#include "error.h"
#include "expected.h"

struct io_uring;
struct epoll_event;

namespace kero {

using Fd = int;

static constexpr Fd kInvalidFd = -1;

class EpollFdDeleter {
 public:
  void operator()(Fd* val) const noexcept;
};

using EpollFd = std::unique_ptr<Fd, EpollFdDeleter>;

class StopEventFdDeleter {
 public:
  void operator()(Fd* val) const noexcept;
};

using StopEventFd = std::unique_ptr<Fd, StopEventFdDeleter>;

class IoUringEventFdDeleter {
 public:
  void operator()(Fd* val) const noexcept;
};

using IoUringEventFd = std::unique_ptr<Fd, IoUringEventFdDeleter>;

class IoUringDeleter {
 public:
  void operator()(io_uring* val) const noexcept;
};

using IoUring = std::unique_ptr<io_uring, IoUringDeleter>;

class EventLoop {
 public:
  expected<void, Error> Run() noexcept;
  expected<void, Error> Stop() const noexcept;

  static expected<EventLoop, Error> Create(const Config& config) noexcept;

 private:
  EventLoop(IoUring&& ring, EpollFd&& epoll_fd, StopEventFd&& stop_event_fd,
            IoUringEventFd&& io_uring_event_fd,
            size_t epoll_events_size) noexcept;

  IoUring ring_;
  EpollFd epoll_fd_;
  StopEventFd stop_event_fd_;
  IoUringEventFd io_uring_event_fd_;

  static_assert(sizeof(ring_) >= sizeof(epoll_fd_));
  static_assert(sizeof(epoll_fd_) >= sizeof(stop_event_fd_));
  static_assert(sizeof(stop_event_fd_) >= sizeof(io_uring_event_fd_));
};

}  // namespace kero

#endif  // KERO_EVENT_LOOP_H_
