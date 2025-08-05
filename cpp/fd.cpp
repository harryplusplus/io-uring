#include "fd.h"

#include <unistd.h>

#include <cerrno>
#include <iostream>

using namespace kero;

void Fd::Close() noexcept {
  if (raw_fd_ == kInvalidRawFd) return;

  const RawFd raw_fd = raw_fd_;
  raw_fd_ = kInvalidRawFd;

  const int ret = ::close(raw_fd);
  if (ret == -1) {
    const int errnum = errno;
    std::cerr << "Failed to close. errnum:" << errnum << ", raw_fd:" << raw_fd
              << ".\n";
  }
}
