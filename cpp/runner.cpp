#include "runner.h"

#include <liburing.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>

using namespace kero;

Result<Fd, Error> create_epoll_fd() noexcept {
  const int ret = ::epoll_create1(EPOLL_CLOEXEC);
  if (ret == -1) return err(errno).reason("Failed to epoll_create1.");

  // if failed. leak.
  if (auto res = Fd::from_raw_fd(ret); !res) {
    return err(Errc::unexpected_error, std::move(res).error().into_cause())
        .reason("Failed to create epoll_fd from raw_fd.");
  }
}

Result<Fd, Error> create_event_fd() noexcept {
  const int ret = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if (ret == -1) return err(errno).reason("Failed to eventfd.");

  // leak.
  return Fd::from_raw_fd(ret);
}

Result<void, Error> add_event_fd_to_epoll(const Fd& epoll_fd,
                                          const Fd& event_fd) noexcept {
  if (!epoll_fd)
    return err(std::errc::invalid_argument).reason("Failed to get epoll_fd.");

  if (!event_fd)
    return err(std::errc::invalid_argument).reason("Failed to get event_fd.");

  struct epoll_event ev{.events = EPOLLIN | EPOLLET, .data = {.fd = *event_fd}};
  if (int ret = epoll_ctl(*epoll_fd, EPOLL_CTL_ADD, *event_fd, &ev); ret == -1)
    return err(errno)
        .detail("epoll_fd", epoll_fd)
        .detail("event_fd", event_fd)
        .reason("Failed to epoll_ctl to add event_fd.");

  return {};
}

Result<void, Error> Runner::stop() const noexcept {
  if (!stop_event_fd_)
    return err(std::errc::bad_file_descriptor)
        .reason("stop_event_fd is invalid.");

  const uint64_t data = 1;
  static_assert(sizeof(data) == 8);
  const ssize_t ret = ::write(*stop_event_fd_, &data, sizeof(data));
  if (ret == -1)
    return err(errno)
        .detail("stop_event_fd_", stop_event_fd_)
        .reason("Failed to write to stop_event_fd.");

  if (ret != sizeof(data))
    return err(Errc::unexpected_error)
        .detail("return_value", ret)
        .reason("Return value of write to stop_event_fd must be 8.");

  return {};
}

Result<Runner, Error> Runner::create(const Config& config) noexcept {
  const int ret = ::epoll_create1(EPOLL_CLOEXEC);
  if (ret == -1) return err(errno).reason("Failed to epoll_create1.");

  // leak.
  if (auto res = Fd::from_raw_fd(ret); !res) {
    return err(Errc::unexpected_error, std::move(res).error().into_cause())
        .reason("Failed to create epoll_fd from raw_fd.");
  }

  else {
    Fd epoll_fd = *std::move(res);

    if (auto res = create_event_fd(); !res) {
      return err(Errc::unexpected_error, std::move(res).error().into_cause())
          .reason("Failed to create event_fd for stop.");
    }

    else {
      Fd stop_event_fd = *std::move(res);

      if (auto res = add_event_fd_to_epoll(epoll_fd, stop_event_fd); !res)
        return err(Errc::unexpected_error, std::move(res).error().into_cause())
            .reason("Failed to add stop_event_fd to epoll.");

      // delete_event_fd. leak.

      auto ring = std::make_unique<struct io_uring>();
      if (int ret = io_uring_queue_init(config.io_uring_queue_entries,
                                        ring.get(), IORING_SETUP_SQPOLL);
          ret < 0)
        return err(-ret)
            .detail("io_uring_queue_entries", config.io_uring_queue_entries)
            .reason("io_uring_queue_init failed.");

      // io_uring_queue_exit. leak.
    }
  }
}
