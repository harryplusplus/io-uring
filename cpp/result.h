#pragma once

#include <system_error>
#include <tl/expected.hpp>

using Error = std::system_error;

template <typename T>
using Result = tl::expected<T, Error>;

inline auto err(Error&& e) noexcept -> tl::unexpected<Error> {
  return tl::unexpected{std::move(e)};
}
