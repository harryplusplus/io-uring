#ifndef KERO_RUNNER_H_
#define KERO_RUNNER_H_

#include <atomic>

#include "config.h"
#include "status.h"
#include "tag.h"

namespace kero {

using RawFd = int;

static constexpr RawFd kInvalidFd = -1;

using EpollFd = Tag<RawFd, struct EpollFdTag>;
using StopEventFd = Tag<RawFd, struct StopEventFdTag>;

class Runner {
 public:
  constexpr Runner(Config&& config) noexcept : config_{std::move(config)} {}

  Status Run() noexcept;
  // Status Stop() const noexcept;

 private:
  // Status WithStopEventFdAdded(const EpollFd& epoll_fd,
  //                             const StopEventFd& stop_event_fd) noexcept;

  std::atomic<StopEventFd> stop_event_fd_{kInvalidFd};
  const Config config_;

  static_assert(sizeof(stop_event_fd_) >= sizeof(config_));
};

}  // namespace kero

#endif  // KERO_RUNNER_H_
