#include "closer.h"

Closer::Closer(std::function<void()>&& fn) noexcept : fn_{std::move(fn)} {
}

Closer::~Closer() noexcept {
  close();
}

void
Closer::close() noexcept {
  if (!is_closed()) {
    auto fn = fn_;
    fn_ = {};
    fn();
  }
}
