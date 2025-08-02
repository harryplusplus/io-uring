#include "epoll.h"

#include "error.h"
#include <sys/epoll.h>

auto Epoll::ctl(int op, const Fd& fd, struct epoll_event& event) const noexcept
    -> Result<void> {
  if (!fd_.is_valid())
    return err({std::errc::bad_file_descriptor, "Failed to call ctl."});

  const int ret = epoll_ctl(fd_.as_raw_fd(), op, fd.as_raw_fd(), &event);
  if (ret == -1)
    return err(
        {create_generic_error_code(errno),
         fmt::format("Failed to call epoll_ctl. op: {}, fd: {}, event: {}", op,
                     fd, event)});

  return {};
}

auto Epoll::pwait(struct epoll_event* events, int maxevents, int timeout,
                  const sigset_t* sigmask) const noexcept -> Result<void> {
  const int ret =
      epoll_pwait(fd_.as_raw_fd(), events, maxevents, timeout, sigmask);
  if (ret == -1)
    return err(
        {create_generic_error_code(errno),
         fmt::format("Failed to call epoll_pwait. maxevents: {}, timeout: {}",
                     maxevents, timeout)});

  return {};
}

auto Epoll::create1(int flags) noexcept -> Result<Epoll> {
  const int ret = epoll_create1(flags);
  if (ret == -1)
    return err({create_generic_error_code(errno),
                fmt::format("Failed to call epoll_create1. flags: {}", flags)});

  return Fd::from_raw_fd(ret).map(
      [](auto&& fd) { return Epoll{std::move(fd)}; });
}

Epoll::Epoll(Fd&& fd) noexcept : fd_(std::move(fd)) {
}
