#ifndef KERO_CLOSE_GUARD_H_
#define KERO_CLOSE_GUARD_H_

#include <functional>

class CloseGuard {
 public:
  CloseGuard() noexcept = default;

  inline CloseGuard(std::function<void()>&& close) noexcept
      : close_{std::move(close)} {}

  CloseGuard(const CloseGuard&) = delete;
  CloseGuard(CloseGuard&&) = delete;

  inline ~CloseGuard() noexcept { Close(); }

  CloseGuard& operator=(const CloseGuard&) = delete;
  CloseGuard& operator=(CloseGuard&&) = delete;

  explicit inline operator bool() const noexcept { return !IsClosed(); }

  inline void Close() noexcept {
    if (!IsClosed()) {
      std::function<void()> close = close_;
      close_ = {};
      close();
    }
  }

  inline bool IsClosed() const noexcept { return !close_; }

 private:
  std::function<void()> close_;
};

#endif  // KERO_CLOSE_GUARD_H_