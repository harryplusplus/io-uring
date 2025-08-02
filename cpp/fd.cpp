#include "fd.h"

#include <fcntl.h>
#include <fmt/format.h>

#include "error.h"

Fd::Fd(Fd&& other) noexcept : raw_fd_{other.raw_fd_} {
  other.raw_fd_ = invalid_raw_fd;
}

Fd::~Fd() noexcept {
  [[maybe_unused]] auto res = close();
}

auto Fd::operator=(Fd&& other) noexcept -> Fd& {
  if (this != &other) {
    [[maybe_unused]] auto res = close();
    raw_fd_ = other.raw_fd_;
    other.raw_fd_ = invalid_raw_fd;
  }

  return *this;
}

auto Fd::close() noexcept -> Result<void> {
  if (!is_valid())
    return {};

  const RawFd raw_fd = raw_fd_;
  raw_fd_ = invalid_raw_fd;

  const int ret = ::close(raw_fd);
  if (ret == -1)
    return err({create_generic_error_code(errno),
                fmt::format("Failed to call close. raw_fd: {}", raw_fd)});

  return {};
}

auto Fd::open(std::string_view pathname, int flags, mode_t mode) noexcept
    -> Result<Fd> {
  const int ret = ::open(pathname.data(), flags, mode);
  if (ret == -1)
    return err(
        {create_generic_error_code(errno),
         fmt::format("Failed to call open. pathname: {}, flags: {}, mode: {}",
                     pathname, flags, mode)});

  return Fd{static_cast<RawFd>(ret)};
}

auto Fd::from_raw_fd(RawFd raw_fd) noexcept -> Result<Fd> {
  if (raw_fd < 0)
    return err({std::errc::bad_file_descriptor,
                fmt::format("Failed to call from_raw_fd. raw_fd: {}", raw_fd)});

  return Fd{raw_fd};
}

Fd::Fd(RawFd raw_fd) noexcept : raw_fd_{raw_fd} {
}
