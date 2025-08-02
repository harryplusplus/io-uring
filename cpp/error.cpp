#include "error.h"

auto KeroCategory::name() const noexcept -> const char* {
  return "kero";
}

auto KeroCategory::message(int condition) const noexcept -> std::string {
  switch (static_cast<kero_errc>(condition)) {
  case kero_errc::invalid_ring:
    return "Invalid ring";
  default:
    return "Unknown error";
  }
}

const KeroCategory& kero_category() noexcept {
  static const KeroCategory instance{};
  return instance;
}
