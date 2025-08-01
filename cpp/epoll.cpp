#include "epoll.h"
#include "fd.h"

#include <fmt/format.h>
#include <sys/epoll.h>

Epoll::~Epoll() noexcept {
  [[maybe_unused]] auto res = close();
}

auto Epoll::close() noexcept -> Result<void> {
  // auto res = fd_.close();
  // if (!res)

  //   if (fd_ == Fd::invalid_value)
  //     return {};

  // const int fd = fd_;
  // fd_ = Fd::invalid_value;

  // const int ret = ::close(fd);
  // if (ret == -1)
  //   return unexpected(
  //       std::system_error{make_error_code(errno), "Failed to close epoll
  //       fd."});

  return {};
}

auto Epoll::create1(int flags) noexcept -> Result<Epoll> {
  const int ret = epoll_create1(flags);
  if (ret == -1)
    return err({make_generic_error_code(errno),
                fmt::format("Failed to call epoll_create1. flags: {}", flags)});

  auto fd = Fd::from_raw_fd(ret);
  if (!fd)
    return err(std::move(fd.error()));

  auto&& a = *fd;
  return tl::expected<Epoll, Error>(Epoll(std::move(a)));
  // return ok(std::move(Epoll(std::move(a))));
}

Epoll::Epoll(Fd&& fd) noexcept : fd_(std::move(fd)) {
}
