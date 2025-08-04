#include "stopper.h"

#include <unistd.h>

using namespace kero;

Result<void, Error> Stopper::stop() const noexcept {
  if (auto ptr = event_fd_.lock(); !ptr) {
    return err(std::errc::no_lock_available)
        .reason("Stopper event fd is closed.");
  }

  else {
    auto&& event_fd = *ptr;
    if (!event_fd) {
      return err(std::errc::bad_file_descriptor)
          .reason("Stopper event fd must valid.");
    }

    uint64_t data = 1;
    static_assert(sizeof(data) == 8);
    ssize_t ret = ::write(*event_fd, &data, sizeof(data));
    if (ret == -1)
      return err(errno)
          .detail("event_fd", event_fd)
          .reason("Failed to write to Stopper event fd.");

    if (ret != sizeof(data))
      return err(kero::Errc::unexpected_error)
          .detail("return_value", ret)
          .reason("Return value of Stopper write must be 8.");

    return {};
  }
}
