#include <fmt/format.h>
#include <liburing.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <tuple>

#include "closer.h"
#include "error.h"
#include "signal_handler.h"

using RawFd = int;

void
print_error(Error err) noexcept {
  if (err)
    fmt::print(stderr, "Error occurred. error: {}", err);
}

[[nodiscard]] Error
close_fd(RawFd fd, std::string&& context) noexcept {
  if (fd < 0)
    return {};

  const int ret = ::close(fd);
  if (ret == -1)
    return Error::errnum(
        errno, fmt::format("close failed. fd: {}, context: {}", fd, context));

  return {};
}

[[nodiscard]] std::tuple<RawFd, Closer, Error>
create_epoll_fd() noexcept {
  const int flags = 0;
  const int ret = epoll_create1(flags);
  if (ret == -1)
    return {RawFd{}, Closer{},
            Error::errnum(
                errno, fmt::format("epoll_create1 failed. flags: {}", flags))};

  return {ret, Closer{[ret]() noexcept {
            print_error(close_fd(ret, "Returned from epoll_create1."));
          }},
          Error{}};
}

[[nodiscard]] std::tuple<RawFd, Closer, Error>
create_event_fd() noexcept {
  const unsigned int count = 0;
  const int flags = EFD_NONBLOCK;
  const int ret = eventfd(count, flags);
  if (ret == -1)
    return {
        RawFd{}, Closer{},
        Error::errnum(errno, fmt::format("eventfd failed. count: {}, flags: {}",
                                         count, flags))};

  return {ret, Closer{[ret]() noexcept {
            print_error(close_fd(ret, "Returned from eventfd."));
          }},
          Error{}};
}

[[nodiscard]] std::tuple<std::shared_ptr<struct io_uring>, Closer, Error>
create_ring() noexcept {
  const uint entries = 8;
  const uint flags = IORING_SETUP_SQPOLL;
  struct io_uring raw_ring {};
  const int ret = io_uring_queue_init(entries, &raw_ring, flags);
  if (ret < 0)
    return {std::make_shared<struct io_uring>(), Closer{},
            Error::errnum(
                -ret, fmt::format(
                          "io_uring_queue_init failed. entries: {}, flags: {}",
                          entries, flags))};

  auto ring = std::make_shared<struct io_uring>(raw_ring);
  return {ring, Closer{[ring]() noexcept { io_uring_queue_exit(ring.get()); }},
          Error{}};
}

[[nodiscard]] Error
unregister_event_fd_from_ring(std::shared_ptr<struct io_uring> ring) noexcept {
  const int ret = io_uring_unregister_eventfd(ring.get());
  if (ret < 0)
    return Error::errnum(-ret,
                         fmt::format("io_uring_unregister_eventfd failed."));

  return {};
}

std::tuple<Closer, Error>
register_event_fd_to_ring(std::shared_ptr<struct io_uring> ring,
                          RawFd event_fd) noexcept {
  const int ret = io_uring_register_eventfd_async(ring.get(), event_fd);
  if (ret < 0)
    return {
        Closer{},
        Error::errnum(
            -ret,
            fmt::format("io_uring_register_eventfd_async failed. event_fd: {}",
                        event_fd))};

  return {Closer{[ring]() noexcept {
            print_error(unregister_event_fd_from_ring(ring));
          }},
          Error{}};
}

[[nodiscard]] Error
unregister_event_fd_from_epoll(RawFd epoll_fd, RawFd event_fd) noexcept {
  const int ret = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event_fd, nullptr);
  if (ret == -1)
    return Error::errnum(errno, fmt::format(""));

  return {};
}

[[nodiscard]] std::tuple<Closer, Error>
register_event_fd_to_epoll(RawFd epoll_fd, RawFd event_fd) noexcept {
  struct epoll_event ev {
    .events = EPOLLIN, .data = {.fd = event_fd }
  };
  const int ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, event_fd, &ev);
  if (ret == -1)
    return {Closer{},
            Error::errnum(
                errno,
                fmt::format(
                    "add_event_fd_to_epoll failed. epoll_fd: {}, event_fd: {}",
                    epoll_fd, event_fd))};

  return {Closer{[epoll_fd, event_fd]() noexcept {
            print_error(unregister_event_fd_from_epoll(epoll_fd, event_fd));
          }},
          Error{}};
}

Error
run() {
  SignalHandler::init();

  auto [epoll_fd, epoll_fd_closer, err] = create_epoll_fd();
  if (err)
    return std::move(err);

  RawFd event_fd{};
  Closer event_fd_closer;
  std::tie(event_fd, event_fd_closer, err) = create_event_fd();
  if (err)
    return std::move(err);

  std::shared_ptr<struct io_uring> ring;
  Closer ring_closer;
  std::tie(ring, ring_closer, err) = create_ring();
  if (err)
    return std::move(err);

  Closer ring_event_fd_unregister;
  std::tie(ring_event_fd_unregister, err) =
      register_event_fd_to_ring(ring, event_fd);
  if (err)
    return std::move(err);

  Closer epoll_event_fd_unregister;
  std::tie(epoll_event_fd_unregister, err) =
      register_event_fd_to_epoll(epoll_fd, event_fd);
  if (err)
    return std::move(err);

  std::array<struct epoll_event, 8> events;
  while (!SignalHandler::is_shutdown_signaled()) {
    const int ret = epoll_wait(epoll_fd, events.data(), events.size(), -1);
    if (ret == -1) {
      const int errnum = errno;
      if (errnum == EINTR) {
        if (SignalHandler::is_shutdown_signaled())
          return {};
        else
          continue;
      } else {
        return Error::errnum(
            errnum, fmt::format("epoll_wait failed. epoll_fd: {}", epoll_fd));
      }
    } else {
      for (int i = 0; i < ret; i++) {
        const auto& event = events[i];
        if (event.data.fd == event_fd) {

        } else {
        }
      }
    }
  }

  return {};
}

int
main() {
  if (auto err = run(); err) {
    fmt::print(stderr, "Run failed. error: {}\n", err);
    return 1;
  }

  // auto fd = Fd::open("test.txt", O_RDWR | O_CREAT, 0600);
  // if (!fd) {
  //   print_error(fd.error());
  //   return 1;
  // }

  // {

  //   const std::string msg{"Hello liburing"};
  //   const struct iovec iov = {
  //       .iov_base = const_cast<char*>(msg.c_str()),
  //       .iov_len = msg.size(),
  //   };

  //   struct io_uring_sqe* sqe = io_uring_get_sqe(&ring);
  //   io_uring_prep_writev(sqe, fd, &iov, 1, 0);
  //   if (int ret = io_uring_submit(&ring); ret < 0) {
  //     std::cerr << "Failed io_uring_submit. code: " << ret << ", "
  //               << strerror(-ret) << std::endl;
  //     return 1;
  //   }
  // }

  // {
  //   struct io_uring_cqe* cqe{};
  //   if (int ret = io_uring_wait_cqe(&ring, &cqe); ret < 0) {
  //     std::cerr << "Failed io_uring_wait_cqe. code: " << ret << ", "
  //               << strerror(-ret) << std::endl;
  //     return 1;
  //   }

  //   const Defer cqe_seen{[&ring, &cqe]() { io_uring_cqe_seen(&ring, cqe); }};

  //   if (cqe->res < 0) {
  //     std::cerr << "Failed io_uring_wait_cqe. code: " << cqe->res << ", "
  //               << strerror(-cqe->res) << std::endl;
  //     return 1;
  //   }
  // }

  // char buffer[100] = {0};
  // const struct iovec iov = {
  //     .iov_base = buffer,
  //     .iov_len = sizeof(buffer) - 1,
  // };
  // struct io_uring_sqe* sqe = io_uring_get_sqe(&ring);
  // io_uring_prep_readv(sqe, fd, &iov, 1, 0);
  // if (int ret = io_uring_submit(&ring); ret < 0) {
  //   std::cerr << "Failed io_uring_submit. code: " << ret << ", "
  //             << strerror(-ret) << std::endl;
  //   return 1;
  // }

  // {
  //   struct io_uring_cqe* cqe{};
  //   if (int ret = io_uring_wait_cqe(&ring, &cqe); ret < 0) {
  //     std::cerr << "Failed io_uring_wait_cqe. code: " << ret << ", "
  //               << strerror(-ret) << std::endl;
  //     return 1;
  //   }

  //   const Defer cqe_seen{[&ring, &cqe]() { io_uring_cqe_seen(&ring, cqe); }};

  //   if (cqe->res < 0) {
  //     std::cerr << "Failed io_uring_wait_cqe. code: " << cqe->res << ", "
  //               << strerror(-cqe->res) << std::endl;
  //     return 1;
  //   }
  // }

  // std::cout << buffer << std::endl;

  // constexpr size_t cqe_count = 8;
  // std::array<struct io_uring_cqe*, cqe_count> cqes{};
  // if (int ret = io_uring_peek_batch_cqe(&ring, cqes.data(), cqes.size());
  //     ret < 0) {
  //   const int errnum = -ret;
  //   if (errnum != EAGAIN) {
  //     std::cerr << "Failed io_uring_peek_batch_cqe. code: " << ret << ", "
  //               << strerror(errnum) << std::endl;
  //     return 1;
  //   }
  // }

  return 0;
}
