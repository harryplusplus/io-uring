#pragma once

#include <fmt/core.h>
#include <string>
#include <system_error>
#include <unordered_map>

namespace kero {

enum class Errc : int {
  unexpected_error = 1000,
};

constexpr bool
is_errc(int code) noexcept {
  return static_cast<int>(Errc::unexpected_error) <= code &&
         code <= static_cast<int>(Errc::unexpected_error);
}

} // namespace kero

namespace std {

constexpr std::string
to_string(kero::Errc code) noexcept {
  switch (code) {
  case kero::Errc::unexpected_error:
    return "unexpected_error";
  }
} // namespace std

template <>
struct is_error_code_enum<kero::Errc> : true_type {};

} // namespace std

namespace kero {

class Category : public std::error_category {
public:
  virtual const char* name() const noexcept override;

  virtual std::string message(int condition) const noexcept override;
};

const Category& category() noexcept;

class Error {
public:
  class Builder {
  public:
    Builder(std::error_code&& ec) noexcept;

    Builder(const Builder&) = delete;
    Builder(Builder&&) = delete;

    ~Builder() noexcept = default;

    Builder& operator=(const Builder&) = delete;
    Builder& operator=(Builder&&) = delete;

    constexpr Builder&&
    reason(std::string&& reason) && noexcept {
      reason_ = std::move(reason);
      return std::move(*this);
    }

    constexpr Builder&&
    detail(std::string&& key, std::string&& value) && noexcept {
      details_.insert(std::move(key), std::move(value));
      return std::move(*this);
    }

    Error build() && noexcept;

  private:
    std::error_code error_code_;
    std::string reason_;
    std::unordered_map<std::string, std::string> details_;

    friend Error;
  };

  Error(Builder&& builder) noexcept;

  Error(const Error&) = delete;
  Error(Error&&) noexcept = default;

  ~Error() noexcept = default;

  Error& operator=(const Error&) = delete;
  Error& operator=(Error&&) noexcept = default;

private:
  std::error_code error_code_;
  std::string reason_;
  std::unordered_map<std::string, std::string> details_;
};

} // namespace kero

namespace std {

std::string to_string(const kero::Error& err) noexcept;

} // namespace std

template <>
struct fmt::formatter<kero::Error> : formatter<string_view> {
  constexpr format_context::iterator
  format(const kero::Error& val, format_context& ctx) {
    return formatter<string_view>::format(std::to_string(val), ctx);
  }
};
