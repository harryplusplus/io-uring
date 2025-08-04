#include "fd.h"

#include <unistd.h>

#include <cerrno>
#include <iostream>

using namespace kero;

void Fd::close() noexcept {
  if (raw_fd_ == invalid_raw_fd) return;

  const RawFd raw_fd = raw_fd_;
  raw_fd_ = invalid_raw_fd;

  const int ret = ::close(raw_fd);
  if (ret == -1) {
    const int errnum = errno;
    // TODO: add on_error handler.
    std::cerr << "close failed. errnum: " << errnum << ", raw_fd: " << raw_fd
              << ".\n";
  }
}
