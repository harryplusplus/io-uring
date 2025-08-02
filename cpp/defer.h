#pragma once

#include <functional>

class Defer {
public:
  explicit Defer(std::function<void()>&& fn) noexcept;
  Defer(const Defer&) = delete;
  Defer(Defer&&) = delete;

  ~Defer() noexcept;

  auto operator=(const Defer&) -> Defer& = delete;
  auto operator=(Defer&&) -> Defer& = delete;

private:
  std::function<void()> fn_;
};
