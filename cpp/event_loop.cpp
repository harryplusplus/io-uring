#include "event_loop.h"

#include <iostream>
#include <liburing.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>

#include "closer.h"

using namespace kero;

EventLoop::~EventLoop() noexcept {
  // TODO
}

Result<void, Error>
EventLoop::run() noexcept {
  int ret = epoll_create1(EPOLL_CLOEXEC);
  if (ret == -1)
    return err(errno).reason("epoll_create1 failed.").build();

  const Fd epoll_fd = ret;
  Closer close_epoll_fd{[epoll_fd]() noexcept {
    const int ret = close(epoll_fd);
    if (ret == -1)
      std::cerr << err(errno)
                       .reason("epoll_fd close failed.")
                       .detail("epoll_fd", epoll_fd)
                       .build()
                << "\n";
  }};

  struct io_uring ring{};
  ret = io_uring_queue_init(8, &ring, IORING_SETUP_SQPOLL);
  if (ret < 0)
    return err(-ret).reason("io_uring_queue_init failed.").build();

  Closer close_ring{[&ring]() noexcept { io_uring_queue_exit(&ring); }};

  ret = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if (ret == -1)
    return err(errno).reason("eventfd for io_uring_event_fd failed.").build();

  const Fd io_uring_event_fd = ret;
  Closer close_io_uring_event_fd{[io_uring_event_fd]() noexcept {
    const int ret = close(io_uring_event_fd);
    if (ret == -1)
      std::cerr << err(errno)
                       .reason("io_uring_event_fd close failed.")
                       .detail("io_uring_event_fd", io_uring_event_fd)
                       .build()
                << "\n";
  }};

  ret = io_uring_register_eventfd_async(&ring, io_uring_event_fd);
  if (ret < 0)
    return err(-ret)
        .reason("io_uring_register_eventfd_async failed.")
        .detail("io_uring_event_fd", io_uring_event_fd)
        .build();

  Closer unregister_event_fd_from_ring{[&ring]() noexcept {
    const int ret = io_uring_unregister_eventfd(&ring);
    if (ret < 0)
      std::cerr
          << err(-ret).reason("io_uring_unregister_eventfd failed.").build()
          << "\n";
  }};

  struct epoll_event ev{.events = EPOLLIN, .data = {.fd = io_uring_event_fd}};
  ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, io_uring_event_fd, &ev);
  if (ret == -1)
    return err(errno)
        .reason("register event fd to epoll failed.")
        .detail("epoll_fd", epoll_fd)
        .detail("io_uring_event_fd", io_uring_event_fd)
        .build();

  Closer unregister_event_fd_from_epoll{
      [epoll_fd, io_uring_event_fd]() noexcept {
        const int ret =
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, io_uring_event_fd, nullptr);
        if (ret == -1)
          std::cerr << err(errno)
                           .reason("unregister event fd from epoll failed.")
                           .detail("epoll_fd", epoll_fd)
                           .detail("io_uring_event_fd", io_uring_event_fd)
                           .build()
                    << "\n";
      }};

  std::array<struct epoll_event, 1024> events;
  while (true) {
    const int ret = epoll_wait(epoll_fd, events.data(), events.size(), -1);
    if (ret == -1) {
      const int errnum = errno;
      if (errnum == EINTR)
        continue;
    }
  }

  return {};
}
