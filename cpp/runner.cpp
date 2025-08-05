#include "runner.h"

#include <liburing.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>

#include <iostream>
#include <memory>

#include "close_guard.h"
#include "tag.h"

using namespace kero;

void CloseFd(RawFd fd) noexcept {
  const int ret = ::close(fd);
  if (ret == -1)
    std::cerr << "Failed to close fd. errnum:" << errno << " fd:" << fd << "\n";
}

int CreateEventFd() noexcept { return eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC); }

int AddEventFd(const EpollFd& epoll_fd, RawFd event_fd) noexcept {
  struct epoll_event ev{.events = EPOLLIN | EPOLLET, .data = {.fd = event_fd}};
  return epoll_ctl(*epoll_fd, EPOLL_CTL_ADD, event_fd, &ev);
}

void DeleteEventFd(const EpollFd& epoll_fd, RawFd event_fd) noexcept {
  const int ret = epoll_ctl(*epoll_fd, EPOLL_CTL_DEL, event_fd, nullptr);
  if (ret == -1)
    std::cerr << "Failed to delete fd from epoll. errnum:" << errno
              << " event_fd:" << event_fd << " epoll_fd:" << *epoll_fd << "\n";
}

Status Runner::WithStopEventFdAdded(const EpollFd& epoll_fd,
                                    const StopEventFd& stop_event_fd) noexcept {
  std::unique_ptr<io_uring> ring = std::make_unique<io_uring>();
  const int ret = io_uring_queue_init(config.io_uring_queue_entries, ring.get(),
                                      IORING_SETUP_SQPOLL);
  return {};
}

Status Runner::WithStopEventFd(const EpollFd& epoll_fd,
                               const StopEventFd& stop_event_fd) noexcept {
  const int ret = AddEventFd(epoll_fd, *stop_event_fd);
  if (ret == -1) return errno;

  CloseGuard close{[&epoll_fd, &stop_event_fd]() noexcept {
    DeleteEventFd(epoll_fd, *stop_event_fd);
  }};

  return WithStopEventFdAdded(epoll_fd, stop_event_fd);
}

Status Runner::WithEpollFd(const EpollFd& epoll_fd) noexcept {
  const int ret = CreateEventFd();
  if (ret == -1) return errno;

  StopEventFd stop_event_fd{ret};
  CloseGuard close{[&stop_event_fd]() noexcept { CloseFd(*stop_event_fd); }};

  return WithStopEventFd(epoll_fd, stop_event_fd);
}

Status Runner::Run() noexcept {
  const int ret = ::epoll_create1(EPOLL_CLOEXEC);
  if (ret == -1) return errno;

  EpollFd epoll_fd{ret};
  CloseGuard close{[&epoll_fd]() noexcept { CloseFd(*epoll_fd); }};

  return WithEpollFd(epoll_fd);
}

Status Runner::Stop() const noexcept {
  if (stop_event_fd_ < 0) return std::errc::bad_file_descriptor;

  const uint64_t data = 1;
  static_assert(sizeof(data) == 8);

  const ssize_t ret = ::write(stop_event_fd_, &data, sizeof(data));
  if (ret == -1) return errno;

  if (ret != sizeof(data)) return ErrorCode::kUnexpectedError;

  return {};
}

// Result<Runner, Error> Runner::create(const Config& config) noexcept {
//   const int ret = ::epoll_create1(EPOLL_CLOEXEC);
//   if (ret == -1) return err(errno).reason("Failed to epoll_create1.");

//   // leak.
//   if (auto res = Fd::from_raw_fd(ret); !res) {
//     return err(Errc::unexpected_error, std::move(res).error().into_cause())
//         .reason("Failed to create epoll_fd from raw_fd.");
//   }

//   else {
//     Fd epoll_fd = *std::move(res);

//     if (auto res = create_event_fd(); !res) {
//       return err(Errc::unexpected_error, std::move(res).error().into_cause())
//           .reason("Failed to create event_fd for stop.");
//     }

//     else {
//       Fd stop_event_fd = *std::move(res);

//       if (auto res = add_event_fd_to_epoll(epoll_fd, stop_event_fd); !res)
//         return err(Errc::unexpected_error,
//         std::move(res).error().into_cause())
//             .reason("Failed to add stop_event_fd to epoll.");

//       // delete_event_fd. leak.

//       auto ring = std::make_unique<struct io_uring>();
//       if (int ret = io_uring_queue_init(config.io_uring_queue_entries,
//                                         ring.get(), IORING_SETUP_SQPOLL);
//           ret < 0)
//         return err(-ret)
//             .detail("io_uring_queue_entries", config.io_uring_queue_entries)
//             .reason("io_uring_queue_init failed.");

//       // io_uring_queue_exit. leak.
//     }
//   }
// }
