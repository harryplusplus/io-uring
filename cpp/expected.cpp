#include "expected.h"

auto make_error_code(int errnum) noexcept -> std::error_code {
  return std::error_code{errnum, std::generic_category()};
}
