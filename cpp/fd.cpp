#include "fd.h"

#include <errno.h>
#include <iostream>
#include <unistd.h>

void
Fd::close() noexcept {
  if (raw_fd_ == invalid_raw_fd)
    return;

  const value_type raw_fd = raw_fd_;
  raw_fd_ = invalid_raw_fd;

  const int ret = ::close(raw_fd);
  if (ret == -1) {
    const int errnum = errno;
    std::cerr << "close failed. errnum: " << errnum << ", raw_fd: " << raw_fd
              << ".\n";
  }
}
