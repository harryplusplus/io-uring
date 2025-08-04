#include "event_loop.h"

#include <iostream>
#include <liburing.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>

#include "closer.h"

using namespace kero;

EventLoop::~EventLoop() noexcept {
  [[maybe_unused]] auto res = stop();
}

Result<void, Error>
EventLoop::run(const Config& config) noexcept {
  if (auto res = open_io_uring(config.io_uring_queue_entries); !res)
    return std::move(res).error();

  if (auto res = open_epoll(); !res)
    return std::move(res).error();

  if (io_uring_event_fd_)
    return err(Errc::already_opened).reason("").build();

  auto event_fd = open_event_fd();

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

Result<void, Error>
EventLoop::stop() noexcept {
  for (auto it = closers_.rbegin(); it != closers_.rend(); it++) {
    auto&& fn = *it;
    if (fn)
      fn();
  }
  closers_.clear();
}

Result<void, Error>
EventLoop::open_io_uring(uint io_uring_queue_entries) noexcept {
  if (ring_)
    return err(Errc::already_opened).reason("io_uring already opened.").build();

  auto ring = std::make_unique<struct io_uring>();
  if (int ret = io_uring_queue_init(io_uring_queue_entries, ring.get(),
                                    IORING_SETUP_SQPOLL);
      ret < 0)
    return err(-ret)
        .reason("io_uring_queue_init failed.")
        .detail("io_uring_queue_entries", io_uring_queue_entries)
        .build();

  closers_.push_back([this]() noexcept {
    if (ring_) {
      io_uring_queue_exit(ring_.get());
      ring_.reset();
    }
  });
  ring_ = std::move(ring);

  return {};
}

Result<void, Error>
EventLoop::open_epoll() noexcept {
  if (epoll_fd_ != invalid_fd)
    return err(Errc::already_opened)
        .reason("epoll already opened.")
        .detail("epoll_fd", epoll_fd_)
        .build();

  const int ret = epoll_create1(EPOLL_CLOEXEC);
  if (ret == -1)
    return err(errno).reason("epoll_create1 failed.").build();

  closers_.push_back([this]() noexcept {
    if (epoll_fd_ != invalid_fd) {
      if (int ret = close(epoll_fd_); ret == -1) {
        std::cerr << "Failed to close epoll fd. errnum: " << errno
                  << ", epoll_fd: " << epoll_fd_ << ".\n";
      }
      epoll_fd_ = invalid_fd;
    }
  });
  epoll_fd_ = ret;

  return {};
}

Result<Fd, Error>
EventLoop::open_event_fd() const noexcept {
  const int ret = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if (ret == -1)
    return err(errno).reason("eventfd failed.").build();

  return ret;
}

Result<void, Error>
EventLoop::open_io_uring_event_fd() noexcept {
  if (io_uring_event_fd_)
    return err(Errc::already_opened)
        .reason("io_uring event fd already opened.")
        .detail("io_uring_event_fd", io_uring_event_fd_)
        .build();

  auto res = open_event_fd();
  if (!res)
    return std::move(res).error();

  Fd event_fd = *std::move(res);
  if (int ret = io_uring_register_eventfd_async(ring_.get(), *event_fd);
      ret < 0)
    return err(-ret)
        .reason("io_uring_register_eventfd_async failed.")
        .detail("event_fd", event_fd)
        .build();

  // unregister

  struct epoll_event ev{.events = EPOLLIN | EPOLLET, .data = {.fd = *event_fd}};
  if (int ret = epoll_ctl(*epoll_fd_, EPOLL_CTL_ADD, *event_fd, &ev); ret == -1)
    return err(errno)
        .reason("Failed to add io_uring event fd to epoll.")
        .build();

  // unregister

  return {};
}
