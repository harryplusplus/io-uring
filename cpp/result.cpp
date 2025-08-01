#include "result.h"
#include "tl/expected.hpp"

auto err(Error&& e) noexcept -> tl::unexpected<Error> {
  return tl::unexpected{std::move(e)};
}

auto make_generic_error_code(int errnum) noexcept -> std::error_code {
  return std::error_code{errnum, std::generic_category()};
}
