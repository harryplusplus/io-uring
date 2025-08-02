#include "event_fd.h"

#include <sys/eventfd.h>

#include "error.h"

auto EventFd::create(unsigned int initval, int flags) noexcept
    -> Result<EventFd> {
  const int ret = eventfd(initval, flags);
  if (ret == -1)
    return err({create_generic_error_code(errno),
                fmt::format("Failed to call eventfd. initval: {}, flags: {}",
                            initval, flags)});

  return Fd::from_raw_fd(ret).map(
      [](auto&& fd) { return EventFd{std::move(fd)}; });
}

EventFd::EventFd(Fd&& fd) noexcept : fd_{std::move(fd)} {
}
