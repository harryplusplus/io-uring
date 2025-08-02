#pragma once

#include <system_error>

inline auto create_generic_error_code(int errnum) noexcept -> std::error_code {
  return {errnum, std::generic_category()};
}

enum class kero_errc : int {
  invalid_ring = 1000,
};

class KeroCategory : public std::error_category {
  virtual const char* name() const noexcept override;

  virtual std::string message(int condition) const noexcept override;
};

auto kero_category() noexcept -> const KeroCategory&;

namespace std {
template <>
struct is_error_code_enum<kero_errc> : true_type {};

template <>
struct is_error_code_enum<std::errc> : true_type {};
} // namespace std

inline auto make_error_code(kero_errc e) noexcept -> std::error_code {
  return {static_cast<int>(e), kero_category()};
}
