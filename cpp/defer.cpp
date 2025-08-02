#include "defer.h"

Defer::Defer(std::function<void()>&& fn) noexcept : fn_{std::move(fn)} {
}

Defer::~Defer() noexcept {
  if (fn_) {
    std::function<void()> fn = fn_;
    fn_ = {};
    fn();
  }
}
