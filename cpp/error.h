#pragma once

#include <fmt/format.h>
#include <string>
#include <unordered_map>

#include "result.h"

enum class KeroErrorCode : int {
  unexpected_error = 1000,
};

constexpr bool
is_kero_error_code(int code) noexcept {
  return static_cast<int>(KeroErrorCode::unexpected_error) <= code &&
         code <= static_cast<int>(KeroErrorCode::unexpected_error);
}

constexpr std::string_view
to_name(KeroErrorCode code) noexcept {
  switch (code) {
  case KeroErrorCode::unexpected_error:
    return "unexpected_error";
  }
}

class Error {
public:
  enum class Category : int {
    errnum = 0,
    kero,
  };

  class Builder {
  public:
    Builder(Category category, int code) noexcept;

    Builder(const Builder&) = delete;
    Builder(Builder&&) = delete;

    ~Builder() noexcept = default;

    Builder& operator=(const Builder&) = delete;
    Builder& operator=(Builder&&) = delete;

    constexpr Builder&&
    message(std::string&& message) && noexcept {
      message_ = std::move(message);
      return std::move(*this);
    }

    constexpr Builder&&
    detail(std::string&& key, std::string&& value) && noexcept {
      details_.insert(std::move(key), std::move(value));
      return std::move(*this);
    }

    Error build() && noexcept;

  private:
    Category category_;
    int code_;
    std::string message_;
    std::unordered_map<std::string, std::string> details_;

    friend Error;
  };

  Error(Builder&& builder) noexcept;

  Error(const Error&) = delete;
  Error(Error&&) noexcept = default;

  ~Error() noexcept = default;

  Error& operator=(const Error&) = delete;
  Error& operator=(Error&&) noexcept = default;

  std::string to_string() const noexcept;

  Category category;
  int code;
  std::string message;
  std::unordered_map<std::string, std::string> details;
};

constexpr std::string_view
to_string(Error::Category category) noexcept {
  switch (category) {
  case Error::Category::errnum:
    return "errnum";
  case Error::Category::kero:
    return "kero";
  }
}

Error::Builder err(int errnum) noexcept;
Error::Builder err(KeroErrorCode code) noexcept;

template <>
struct fmt::formatter<Error> : formatter<string_view> {
  constexpr format_context::iterator
  format(const Error& val, format_context& ctx) {
    return formatter<string_view>::format(val.to_string(), ctx);
  }
};

template <typename T>
using Result = kero::Result<T, Error>;

template <typename T>
constexpr Result<T>
ok(T&& val) noexcept {
  return kero::ok<T, Error>(std::move(val));
}

template <typename T>
constexpr Result<T>
err(Error&& err) noexcept {
  return kero::err<T, Error>(std::move(err));
}

constexpr Result<void>
ok() noexcept {
  return kero::ok<Error>();
}
