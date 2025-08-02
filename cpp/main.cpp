#include <fmt/format.h>
#include <liburing.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>

#include "fd.h"

auto run() -> Result<void> {
  Fd epoll_fd;
  {
    const int flags = 0;
    const int ret = epoll_create1(flags);
    if (ret == -1)
      return err(Error::errnum(
          errno, fmt::format("epoll_create1 failed. flags: {}", flags)));

    auto fd = Fd::from_raw_fd(ret);
    if (!fd)
      return err(std::move(fd.error()));

    epoll_fd = std::move(*fd);
  }

  Fd event_fd;
  {
    const unsigned int count = 0;
    const int flags = EFD_NONBLOCK;
    const int ret = eventfd(count, flags);
    if (ret == -1)
      return err(Error::errnum(
          errno,
          fmt::format("eventfd failed. count: {}, flags: {}", count, flags)));

    auto fd = Fd::from_raw_fd(ret);
    if (!fd)
      return err(std::move(fd.error()));

    event_fd = std::move(*fd);
  }

  auto io_uring = IoUring::queue_init(0, IORING_SETUP_SQPOLL);
  if (!io_uring)
    return err(std::move(io_uring.error()));

  io_uring_register_eventfd_async(&io_uring->as_raw_ring(),
                                  event_fd.as_raw_fd());

  return {};
}

auto main() -> int {
  if (auto res = run(); !res) {
    fmt::print(stderr, "{}", res.error());
    return 1;
  }

  if (auto res = io_uring->register_eventfd_async(*event_fd); !res) {
    print_error(res.error());
    return 1;
  }

  struct epoll_event ev {
    .events = EPOLLIN
  };
  ev.data.fd = event_fd->as_fd().as_raw_fd();
  if (auto res = epoll->ctl(EPOLL_CTL_ADD, event_fd->as_fd(), ev); !res) {
    print_error(res.error());
    return 1;
  }

  auto fd = Fd::open("test.txt", O_RDWR | O_CREAT, 0600);
  if (!fd) {
    print_error(fd.error());
    return 1;
  }

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
