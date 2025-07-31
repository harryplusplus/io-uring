#pragma once

#include <memory>
#include <tl/expected.hpp>

using Error = std::unique_ptr<std::exception>;

template <typename T> using Result = tl::expected<T, Error>;

template <typename E> auto Err(E &&e) noexcept {
  return tl::unexpected(std::make_unique<E>(std::move(e)));
}
