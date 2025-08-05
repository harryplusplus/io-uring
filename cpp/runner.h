#ifndef KERO_RUNNER_H_
#define KERO_RUNNER_H_

#include "config.h"
#include "fd.h"

namespace kero {

class Runner {
 public:
  Result<void, Error> run() noexcept;
  Result<void, Error> stop() const noexcept;

  static Result<Runner, Error> create(const Config& config) noexcept;

 private:
  inline Runner(Fd&& epoll_fd, Fd&& stop_event_fd) noexcept
      : epoll_fd_{std::move(epoll_fd)},
        stop_event_fd_{std::move(stop_event_fd)} {
    assert(epoll_fd_);
    assert(stop_event_fd_);
  }

  Fd epoll_fd_;
  Fd stop_event_fd_;

  static_assert(sizeof(epoll_fd_) >= sizeof(stop_event_fd_));
};

}  // namespace kero

#endif  // KERO_RUNNER_H_
