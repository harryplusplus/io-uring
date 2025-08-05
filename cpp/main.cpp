#include <iostream>

#include "status.h"
// #include "event_loop.h"
// #include "result.h"
// #include "signal_handler.h"

using namespace kero;

int main() {
  // std::shared_ptr<Runner> runner = Runner::Create();
  // StopGuard stop(runner);
  // // runner.Run();
  // signal.on(SIGINT, [runner]() { runner.stop(); });
  // auto res =
  //     With([]() noexcept {}, []() noexcept {}, [](resource) { return {}; });
  // SignalHandler::init();

  // auto res = EventLoop::create();
  // auto [runner, stopper] = *std::move(res);
  // auto t = std::jthread([]() { runner.run(); });
  // stopper.stop();

  // if (auto err = run(); err) {
  //   fmt::print(stderr, "Run failed. error: {}\n", err);
  //   return 1;
  // }

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

  //   const Defer cqe_seen{[&ring, &cqe]() { io_uring_cqe_seen(&ring, cqe);
  //   }};

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

  //   const Defer cqe_seen{[&ring, &cqe]() { io_uring_cqe_seen(&ring, cqe);
  //   }};

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
  //     std::cerr << "Failed io_uring_peek_batch_cqe. code: " << ret << ",
  //     "
  //               << strerror(errnum) << std::endl;
  //     return 1;
  //   }
  // }

  return 0;
}
