#include "raw_fd_deleter.h"

#include <unistd.h>

#include <cerrno>
#include <iostream>

using namespace kero;

void RawFdDeleter::operator()(RawFd* val) const noexcept {
  if (!val) return;

  const RawFd raw_fd = *val;
  delete val;

  if (raw_fd <= invalid_raw_fd) return;

  const int ret = ::close(raw_fd);
  if (ret == -1) {
    const int errnum = errno;
    if (on_error_) on_error_(raw_fd, errnum);
  }
}

void RawFdDeleter::on_error(RawFd raw_fd, int errnum) noexcept {
  std::cerr << "Failed to close raw_fd. errnum:" << errnum
            << " raw_fd:" << raw_fd << "\n";
}
