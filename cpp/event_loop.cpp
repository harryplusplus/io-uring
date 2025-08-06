#include "event_loop.h"

#include <liburing.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>

#include <iostream>
#include <memory>

using namespace kero;

void CloseFd(Fd fd) noexcept {
  const int ret = ::close(fd);
  if (ret == -1) {
    const int errnum = errno;
    std::cerr << "Failed to close. errnum:" << errnum << ", fd:" << fd << ".\n";
  }
}

void DeleteFd(Fd* fd) noexcept {
  if (!fd) return;
  CloseFd(*fd);
  delete fd;
}

void EpollFdDeleter::operator()(Fd* val) const noexcept { DeleteFd(val); }

void StopEventFdDeleter::operator()(Fd* val) const noexcept { DeleteFd(val); }

void IoUringEventFdDeleter::operator()(Fd* val) const noexcept {
  DeleteFd(val);
}

void IoUringDeleter::operator()(io_uring* val) const noexcept {
  if (!val) return;
  if (int ret = io_uring_unregister_eventfd(val); ret < 0)
    std::cerr << "Failed to io_uring_unregister_eventfd. errnum:" << -ret
              << ".\n";
  io_uring_queue_exit(val);
  delete val;
}

expected<EpollFd, Error> CreateEpollFd() noexcept {
  const int ret = epoll_create1(EPOLL_CLOEXEC);
  if (ret == -1) return unexpected{errno};
  return EpollFd{new Fd{ret}};
}

expected<Fd, Error> CreateEventFd() noexcept {
  const int ret = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if (ret == -1) return unexpected{errno};
  return Fd{ret};
}

expected<StopEventFd, Error> CreateStopEventFd() noexcept {
  auto fd = CreateEventFd();
  if (!fd) return unexpected{std::move(fd).error()};
  return StopEventFd{new Fd{*fd}};
}

expected<IoUringEventFd, Error> CreateIoUringEventFd() noexcept {
  auto fd = CreateEventFd();
  if (!fd) return unexpected{std::move(fd).error()};
  return IoUringEventFd{new Fd{*fd}};
}

expected<void, Error> AddEventFd(const EpollFd& epoll_fd,
                                 Fd event_fd) noexcept {
  struct epoll_event ev{.events = EPOLLIN | EPOLLET, .data = {.fd = event_fd}};
  const int ret = epoll_ctl(*epoll_fd, EPOLL_CTL_ADD, event_fd, &ev);
  if (ret == -1) return unexpected{errno};
  return {};
}

expected<IoUring, Error> CreateIoUring(uint io_uring_queue_entries) noexcept {
  IoUring ring = IoUring{new io_uring{}};
  const int ret = io_uring_queue_init(io_uring_queue_entries, ring.get(),
                                      IORING_SETUP_SQPOLL);
  if (ret < 0) return unexpected{-ret};
  return ring;
}

expected<void, Error> RegisterIoUringEventFd(
    const IoUring& ring, const IoUringEventFd& io_uring_event_fd) noexcept {
  const int ret =
      io_uring_register_eventfd_async(ring.get(), *io_uring_event_fd);
  if (ret < 0) return unexpected{-ret};
  return {};
}

expected<void, Error> EventLoop::Run() noexcept {
  while (true) {
    const int ret =
        epoll_wait(*epoll_fd_, epoll_events_.data(), epoll_events_.size(), -1);
    if (ret == -1) {
      const int errnum = errno;
      if (errnum == EINTR) continue;

      return unexpected{errnum};
    }

    for (int i = 0; i < ret; i++) {
      auto&& ev = epoll_events_[i];
      if (ev.data.fd == *stop_event_fd_) {
      } else if (ev.data.fd == *io_uring_event_fd_) {
      } else {
        std::cerr << "Unexpected epoll event. u64:" << ev.data.u64
                  << ", events:" << ev.events << "\n";
      }
    }
  }
}

expected<void, Error> EventLoop::Stop() const noexcept {
  // TODO
  return {};
}

// Status Runner::Stop() const noexcept {
//   if (stop_event_fd_ < 0) return std::errc::bad_file_descriptor;

//   const uint64_t data = 1;
//   static_assert(sizeof(data) == 8);

//   const ssize_t ret = ::write(stop_event_fd_, &data, sizeof(data));
//   if (ret == -1) return errno;

//   if (ret != sizeof(data)) return ErrorCode::kUnexpectedError;

//   return {};
// }

expected<EventLoop, Error> EventLoop::Create(const Config& config) noexcept {
  auto epoll_fd = CreateEpollFd();
  if (!epoll_fd) return unexpected{std::move(epoll_fd).error()};

  auto stop_event_fd = CreateStopEventFd();
  if (!stop_event_fd) return unexpected{std::move(stop_event_fd).error()};

  if (auto res = AddEventFd(*epoll_fd, **stop_event_fd); !res)
    return unexpected{std::move(res).error()};

  auto ring = CreateIoUring(config.io_uring_queue_entries);
  if (!ring) return unexpected{std::move(ring).error()};

  auto io_uring_event_fd = CreateIoUringEventFd();
  if (!io_uring_event_fd)
    return unexpected{std::move(io_uring_event_fd).error()};

  if (auto res = RegisterIoUringEventFd(*ring, *io_uring_event_fd); !res)
    return unexpected{std::move(res).error()};

  if (auto res = AddEventFd(*epoll_fd, **io_uring_event_fd); !res)
    return unexpected{std::move(res).error()};

  return EventLoop{*std::move(ring), *std::move(epoll_fd),
                   *std::move(stop_event_fd), *std::move(io_uring_event_fd),
                   config.epoll_events_size};
}

EventLoop::EventLoop(IoUring&& ring, EpollFd&& epoll_fd,
                     StopEventFd&& stop_event_fd,
                     IoUringEventFd&& io_uring_event_fd,
                     size_t epoll_events_size) noexcept
    : ring_{std::move(ring)},
      epoll_fd_{std::move(epoll_fd)},
      stop_event_fd_{std::move(stop_event_fd)},
      io_uring_event_fd_{std::move(io_uring_event_fd)} {}
