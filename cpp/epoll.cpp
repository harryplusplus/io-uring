#include "epoll.h"
#include <cerrno>
#include <exception>
#include <memory>
#include <sys/epoll.h>
#include <system_error>
#include <unistd.h>

Epoll::~Epoll() noexcept { [[maybe_unused]] auto res = close(); }

auto Epoll::close() noexcept -> Result<void> {
  if (fd_ == invalid_fd)
    return {};

  const int fd = fd_;
  fd_ = invalid_fd;

  const int ret = ::close(fd);
  if (ret == -1)
    return Err(
        std::system_error{std::error_code{errno, std::generic_category()},
                          "Failed to close epoll fd."});

  return {};
}

auto Epoll::create1(int flags) noexcept -> Result<Epoll> {
  const int fd = epoll_create1(flags);
  if (fd == -1)
    return Err(
        std::system_error{std::error_code{errno, std::generic_category()},
                          "Failed to epoll_create1."});

  return Epoll{fd};
}

Epoll::Epoll(int fd) noexcept : fd_(fd) {}
