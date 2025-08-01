#include "epoll.h"
#include "io_uring.h"
#include <exception>
#include <functional>
#include <iostream>
#include <liburing.h>
#include <optional>
#include <string.h>
#include <system_error>

class Defer {
public:
  explicit Defer(std::function<void()>&& fn) noexcept : fn_(std::move(fn)) {
  }

  Defer(const Defer&) = delete;
  Defer(Defer&&) = delete;

  Defer& operator=(const Defer&) = delete;
  Defer& operator=(Defer&&) = delete;

  ~Defer() noexcept {
    if (fn_) {
      fn_();
      fn_ = {};
    }
  }

private:
  std::function<void()> fn_;
};

auto print_error(const Error& e) -> void {
  std::cerr << "code: " << e.code().value()
            << ", message: " << e.code().message()
            << ", category: " << e.code().category().name()
            << ", what: " << e.what() << std::endl;
}

int main() {
  auto epoll = Epoll::create1(0);
  if (!epoll) {
    print_error(epoll.error());
    return 1;
  }

  struct io_uring ring {};
  constexpr size_t entries = 8;
  auto io_uring = IoUring::queue_init(8, IORING_SETUP_SQPOLL);
  if (!io_uring) {
    print_error(io_uring.error());
    return 1;
  }

  const int fd = open("test.txt", O_RDWR | O_CREAT, 0600);
  if (fd < 0) {
    std::cerr << "Failed to open file." << std::endl;
    return 1;
  }
  const Defer close_fd{[fd]() { close(fd); }};

  {

    const std::string msg{"Hello liburing"};
    const struct iovec iov = {
        .iov_base = const_cast<char*>(msg.c_str()),
        .iov_len = msg.size(),
    };

    struct io_uring_sqe* sqe = io_uring_get_sqe(&ring);
    io_uring_prep_writev(sqe, fd, &iov, 1, 0);
    if (int ret = io_uring_submit(&ring); ret < 0) {
      std::cerr << "Failed io_uring_submit. code: " << ret << ", "
                << strerror(-ret) << std::endl;
      return 1;
    }
  }

  {
    struct io_uring_cqe* cqe{};
    if (int ret = io_uring_wait_cqe(&ring, &cqe); ret < 0) {
      std::cerr << "Failed io_uring_wait_cqe. code: " << ret << ", "
                << strerror(-ret) << std::endl;
      return 1;
    }

    const Defer cqe_seen{[&ring, &cqe]() { io_uring_cqe_seen(&ring, cqe); }};

    if (cqe->res < 0) {
      std::cerr << "Failed io_uring_wait_cqe. code: " << cqe->res << ", "
                << strerror(-cqe->res) << std::endl;
      return 1;
    }
  }

  char buffer[100] = {0};
  const struct iovec iov = {
      .iov_base = buffer,
      .iov_len = sizeof(buffer) - 1,
  };
  struct io_uring_sqe* sqe = io_uring_get_sqe(&ring);
  io_uring_prep_readv(sqe, fd, &iov, 1, 0);
  if (int ret = io_uring_submit(&ring); ret < 0) {
    std::cerr << "Failed io_uring_submit. code: " << ret << ", "
              << strerror(-ret) << std::endl;
    return 1;
  }

  {
    struct io_uring_cqe* cqe{};
    if (int ret = io_uring_wait_cqe(&ring, &cqe); ret < 0) {
      std::cerr << "Failed io_uring_wait_cqe. code: " << ret << ", "
                << strerror(-ret) << std::endl;
      return 1;
    }

    const Defer cqe_seen{[&ring, &cqe]() { io_uring_cqe_seen(&ring, cqe); }};

    if (cqe->res < 0) {
      std::cerr << "Failed io_uring_wait_cqe. code: " << cqe->res << ", "
                << strerror(-cqe->res) << std::endl;
      return 1;
    }
  }

  std::cout << buffer << std::endl;

  constexpr size_t cqe_count = 8;
  std::array<struct io_uring_cqe*, cqe_count> cqes{};
  if (int ret = io_uring_peek_batch_cqe(&ring, cqes.data(), cqes.size());
      ret < 0) {
    const int errnum = -ret;
    if (errnum != EAGAIN) {
      std::cerr << "Failed io_uring_peek_batch_cqe. code: " << ret << ", "
                << strerror(errnum) << std::endl;
      return 1;
    }
  }

  return 0;
}
