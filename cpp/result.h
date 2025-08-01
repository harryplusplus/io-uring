#pragma once

#include <system_error>
#include <tl/expected.hpp>

using Error = std::system_error;

template <typename T> using Result = tl::expected<T, Error>;

template <typename T> auto ok(T&& v) noexcept {
  return Result<T>(std::move(v));
}

auto err(Error&& e) noexcept -> tl::unexpected<Error>;

auto make_generic_error_code(int errnum) noexcept -> std::error_code;
