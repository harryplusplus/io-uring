#include "epoll.h"
#include <cerrno>
#include <exception>
#include <memory>
#include <sys/epoll.h>
#include <system_error>
#include <unistd.h>

Epoll::~Epoll() noexcept { [[maybe_unused]] auto res = close(); }

auto Epoll::close() noexcept -> Expected<void> {
  if (fd_ == invalid_fd)
    return {};

  const int fd = fd_;
  fd_ = invalid_fd;

  const int ret = ::close(fd);
  if (ret == -1)
    return unexpected(
        std::system_error{make_error_code(errno), "Failed to close epoll fd."});

  return {};
}

auto Epoll::create1(int flags) noexcept -> Expected<Epoll> {
  const int fd = epoll_create1(flags);
  if (fd == -1)
    return unexpected(std::system_error{make_error_code(errno),
                                        "Failed to call epoll_create1."});

  return Epoll{fd};
}

Epoll::Epoll(int fd) noexcept : fd_(fd) {}
