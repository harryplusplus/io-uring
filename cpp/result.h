#pragma once

#include <tl/expected.hpp>

#include "error.h"

template <typename T>
using Result = tl::expected<T, Error>;

constexpr auto err(Error e) noexcept -> tl::unexpected<Error> {
  return tl::unexpected{std::move(e)};
}
