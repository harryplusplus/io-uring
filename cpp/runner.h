#ifndef KERO_RUNNER_H_
#define KERO_RUNNER_H_

#include "fd.h"

namespace kero {

class Runner {
 public:
  static Result<Runner, Error> create() noexcept;

 private:
  inline Runner(std::shared_ptr<Fd>&& stop_event_fd, Fd epoll_fd) noexcept
      : stop_event_fd_{std::move(stop_event_fd)},
        epoll_fd_{std::move(epoll_fd)} {
    assert(stop_event_fd_);
    assert(*stop_event_fd_);
    assert(epoll_fd_);
  }

  std::shared_ptr<Fd> stop_event_fd_;
  Fd epoll_fd_;

  static_assert(sizeof(stop_event_fd_) >= sizeof(epoll_fd_));
};

}  // namespace kero

#endif  // KERO_RUNNER_H_
