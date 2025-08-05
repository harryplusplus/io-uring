#ifndef KERO_CLOSE_GUARD_H_
#define KERO_CLOSE_GUARD_H_

#include <functional>

template <std::invocable F>
  requires std::is_nothrow_invocable_r_v<void, F>
class CloseGuard {
 public:
  constexpr CloseGuard(F&& on_close) noexcept
      : on_close_{std::forward<F>(on_close)}, is_closed_{false} {}

  CloseGuard(const CloseGuard&) = delete;
  CloseGuard(CloseGuard&&) = delete;

  constexpr ~CloseGuard() noexcept { Close(); }

  CloseGuard& operator=(const CloseGuard&) = delete;
  CloseGuard& operator=(CloseGuard&&) = delete;

  explicit constexpr operator bool() const noexcept { return !IsClosed(); }

  constexpr void Close() noexcept {
    if (!IsClosed()) {
      is_closed_ = true;
      std::invoke(std::move(on_close_));
    }
  }

  constexpr bool IsClosed() const noexcept { return is_closed_; }

 private:
  F on_close_;
  bool is_closed_;

  static_assert(sizeof(on_close_) >= sizeof(is_closed_));
};

#endif  // KERO_CLOSE_GUARD_H_