#pragma once

#include <memory>
#include <tl/expected.hpp>

template <typename T>
using Expected = tl::expected<T, std::unique_ptr<std::exception>>;

template <typename E> auto unexpected(E &&e) noexcept {
  return tl::unexpected{std::make_unique<E>(std::move(e))};
}

auto make_error_code(int errnum) noexcept -> std::error_code;
