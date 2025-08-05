#ifndef KERO_RAW_FD_DELETER_H_
#define KERO_RAW_FD_DELETER_H_

#include <functional>
#include <memory>

namespace kero {

using RawFd = int;

static constexpr RawFd invalid_raw_fd = -1;

class RawFdDeleter {
 public:
  using OnError = std::function<void(RawFd, int)>;

  RawFdDeleter() noexcept = default;

  inline RawFdDeleter(OnError&& on_error) noexcept
      : on_error_{std::move(on_error)} {}

  RawFdDeleter(const RawFdDeleter&) = delete;
  RawFdDeleter(RawFdDeleter&&) noexcept = default;

  ~RawFdDeleter() noexcept = default;

  RawFdDeleter& operator=(const RawFdDeleter&) = delete;
  RawFdDeleter& operator=(RawFdDeleter&&) noexcept = default;

  void operator()(RawFd* val) const noexcept;

 private:
  static void on_error(RawFd raw_fd, int errnum) noexcept;

  OnError on_error_{on_error};
};

}  // namespace kero

#endif  // KERO_RAW_FD_DELETER_H_
