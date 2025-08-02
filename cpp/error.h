#pragma once

#include <fmt/format.h>
#include <string>

constexpr std::string_view
errnum_to_name(int errnum) noexcept {
  const char* name = strerrorname_np(errnum);
  return name ? name : "";
};

constexpr std::string_view
errnum_to_desc(int errnum) noexcept {
  const char* desc = strerrordesc_np(errnum);
  return desc ? desc : "";
}

enum class ErrorCategory : int {
  undefined = 0,
  errnum,
  kero,
};

constexpr std::string_view
to_string(ErrorCategory category) noexcept {
  switch (category) {
  case ErrorCategory::undefined:
    return "undefined";
  case ErrorCategory::errnum:
    return "errnum";
  case ErrorCategory::kero:
    return "kero";
  }
}

enum class KeroErrorCode : int {
  invalid_fd = 1000,
  invalid_io_uring,
};

constexpr std::string_view
to_name(KeroErrorCode code) noexcept {
  switch (code) {
  case KeroErrorCode::invalid_fd:
    return "invalid_fd";
  case KeroErrorCode::invalid_io_uring:
    return "invalid_io_uring";
  }
}

class Error {
public:
  Error() noexcept = default;
  Error(const Error&) = delete;
  Error(Error&&) noexcept = default;

  ~Error() noexcept = default;

  Error& operator=(const Error&) = delete;
  Error& operator=(Error&&) noexcept = default;

  constexpr explicit operator bool() const noexcept {
    return has_value();
  }

  constexpr bool
  has_value() const noexcept {
    return category_ != ErrorCategory::undefined;
  }

  constexpr ErrorCategory
  category() const noexcept {
    return category_;
  }

  constexpr int
  code() const noexcept {
    return code_;
  }

  constexpr std::string_view
  message() const noexcept {
    return message_;
  }

  constexpr std::string
  to_string() const noexcept {
    std::string ret{fmt::format("Error{{category:{},code:{},",
                                ::to_string(category_), code_)};

    switch (category_) {
    case ErrorCategory::undefined:
      break;
    case ErrorCategory::errnum: {
      std::string_view name = errnum_to_name(code_);
      if (!name.empty())
        ret += fmt::format("name:{},", name);
      std::string_view desc = errnum_to_desc(code_);
      if (!desc.empty())
        ret += fmt::format("desc:{},", desc);
    } break;
    case ErrorCategory::kero: {
      ret +=
          fmt::format("name:{},", to_name(static_cast<KeroErrorCode>(code_)));
    } break;
    }

    ret += fmt::format("message:{}}}", message_);
    return ret;
  }

  static constexpr Error
  errnum(int errnum, std::string message) noexcept {
    return Error{ErrorCategory::errnum, errnum, std::move(message)};
  }

  static constexpr Error
  kero(KeroErrorCode code, std::string message) noexcept {
    return Error{ErrorCategory::kero, static_cast<int>(code),
                 std::move(message)};
  }

private:
  constexpr Error(ErrorCategory category, int code,
                  std::string message) noexcept
      : category_{category}, code_{code}, message_{std::move(message)} {
  }

  ErrorCategory category_{ErrorCategory::undefined};
  int code_{};
  std::string message_;
};

template <>
struct fmt::formatter<Error> : formatter<string_view> {
  constexpr format_context::iterator
  format(const Error& val, format_context& ctx) {
    return formatter<string_view>::format(val.to_string(), ctx);
  }
};
