#include "runtime.h"

#include <iostream>
#include <liburing.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>

#include "closer.h"

using namespace kero;

using Fd = int;

[[nodiscard]] Result<void, Error>
close_fd(Fd fd) noexcept {
  if (fd < 0)
    return {};

  const int ret = close(fd);
  if (ret == -1)
    return err(errno).reason("close failed.").detail("fd", fd).build();

  return {};
}

Result<void, Error>
Runtime::run() noexcept {
  int ret = epoll_create1(0);
  if (ret == -1)
    return err(errno).reason("epoll_create1 failed.").build();

  const Fd epoll_fd = ret;
  Closer close_epoll_fd{[epoll_fd]() noexcept {
    if (auto res = close_fd(epoll_fd); !res)
      std::cerr << res.error().add_detail("epoll_fd", epoll_fd) << "\n";
  }};

  ret = eventfd(0, EFD_NONBLOCK);
  if (ret == -1)
    return err(errno).reason("eventfd failed.").build();

  const Fd event_fd = ret;
  Closer close_event_fd{[event_fd]() noexcept {
    if (auto res = close_fd(event_fd); !res) {
      std::cerr << res.error().add_detail("event_fd", event_fd) << "\n";
    }
  }};

  struct io_uring ring{};
  ret = io_uring_queue_init(8, &ring, IORING_SETUP_SQPOLL);
  if (ret < 0)
    return err(-ret).reason("io_uring_queue_init failed.").build();

  Closer close_ring{[&ring]() noexcept { io_uring_queue_exit(&ring); }};

  ret = io_uring_register_eventfd_async(&ring, event_fd);
  if (ret < 0)
    return err(-ret)
        .reason("io_uring_register_eventfd_async failed.")
        .detail("event_fd", event_fd)
        .build();

  Closer unregister_event_fd_from_ring{[&ring]() noexcept {
    const int ret = io_uring_unregister_eventfd(&ring);
    if (ret < 0)
      std::cerr
          << err(-ret).reason("io_uring_unregister_eventfd failed.").build()
          << "\n";
  }};

  struct epoll_event ev{.events = EPOLLIN, .data = {.fd = event_fd}};
  ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, event_fd, &ev);
  if (ret == -1)
    return err(errno)
        .reason("register event fd to epoll failed.")
        .detail("epoll_fd", epoll_fd)
        .detail("event_fd", event_fd)
        .build();

  Closer unregister_event_fd_from_epoll{[epoll_fd, event_fd]() noexcept {
    const int ret = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event_fd, nullptr);
    if (ret == -1)
      std::cerr << err(errno)
                       .reason("unregister event fd from epoll failed.")
                       .detail("epoll_fd", epoll_fd)
                       .detail("event_fd", event_fd)
                       .build()
                << "\n";
  }};

  return {};
}
