#pragma once

#include <functional>

class Closer {
public:
  Closer() noexcept = default;
  Closer(std::function<void()>&& fn) noexcept;
  Closer(const Closer&) = delete;
  Closer(Closer&&) noexcept = default;

  ~Closer() noexcept;

  Closer& operator=(const Closer&) = delete;
  Closer& operator=(Closer&&) noexcept = default;

  explicit constexpr operator bool() const noexcept {
    return !is_closed();
  }

  void close() noexcept;

  constexpr bool
  is_closed() const noexcept {
    return !fn_;
  }

private:
  std::function<void()> fn_;
};
