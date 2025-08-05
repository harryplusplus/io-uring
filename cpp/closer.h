#pragma once

#include <functional>

class Closer {
 public:
  Closer() noexcept = default;

  inline Closer(std::function<void()>&& fn) noexcept : fn_{std::move(fn)} {}

  Closer(const Closer&) = delete;
  Closer(Closer&&) noexcept = default;

  inline ~Closer() noexcept { close(); }

  Closer& operator=(const Closer&) = delete;
  Closer& operator=(Closer&&) noexcept = default;

  explicit constexpr operator bool() const noexcept { return !is_closed(); }

  inline void close() noexcept {
    if (!is_closed()) {
      auto fn = fn_;
      fn_ = {};
      fn();
    }
  }

  constexpr bool is_closed() const noexcept { return !fn_; }

 private:
  std::function<void()> fn_;
};
