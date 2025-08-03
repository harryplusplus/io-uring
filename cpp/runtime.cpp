#include "runtime.h"

#include <unistd.h>

using namespace kero;

using Fd = int;

[[nodiscard]] Result<void, Error>
close_fd(Fd fd) noexcept {
  if (fd < 0)
    return {};

  const int ret = ::close(fd);
  if (ret == -1)
    return err(errno).reason("close failed.").detail("fd", fd).build();

  return {};
}

Result<void, Error>
Runtime::run() noexcept {
  return {};
}
