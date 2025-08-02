#pragma once

#include <fmt/format.h>
#include <string>

constexpr auto errnum_to_name(int errnum) noexcept -> std::string_view {
  const char* name = strerrorname_np(errnum);
  return name ? name : "";
};

constexpr auto errnum_to_desc(int errnum) noexcept -> std::string_view {
  const char* desc = strerrordesc_np(errnum);
  return desc ? desc : "";
}

enum class ErrorCategory : int {
  errnum = 0,
  kero,
};

constexpr auto to_string(ErrorCategory category) noexcept -> std::string_view {
  switch (category) {
  case ErrorCategory::errnum:
    return "errnum";
  case ErrorCategory::kero:
    return "kero";
  default:
    return "unknown";
  }
}

enum class KeroErrorCode : int {
  invalid_fd = 1000,
  invalid_io_uring,
};

constexpr auto to_name(KeroErrorCode code) noexcept -> std::string_view {
  switch (code) {
  case KeroErrorCode::invalid_fd:
    return "invalid_fd";
  case KeroErrorCode::invalid_io_uring:
    return "invalid_io_uring";
  default:
    return "unknown";
  }
}

class Error {
public:
  Error(const Error&) = delete;
  Error(Error&&) = default;

  ~Error() noexcept = default;

  auto operator=(const Error&) -> Error& = delete;
  auto operator=(Error&&) noexcept -> Error& = default;

  constexpr auto category() const noexcept -> ErrorCategory {
    return category_;
  }

  constexpr auto code() const noexcept -> int {
    return code_;
  }

  constexpr auto message() const noexcept -> std::string_view {
    return message_;
  }

  constexpr auto to_string() const noexcept -> std::string {
    std::string ret{fmt::format("Error{{category:{},code:{},",
                                ::to_string(category_), code_)};

    switch (category_) {
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

  static constexpr auto errnum(int errnum, std::string message) noexcept
      -> Error {
    return Error{ErrorCategory::errnum, errnum, std::move(message)};
  }

  static constexpr auto kero(KeroErrorCode code, std::string message) noexcept
      -> Error {
    return Error{ErrorCategory::kero, static_cast<int>(code),
                 std::move(message)};
  }

private:
  constexpr Error(ErrorCategory category, int code,
                  std::string message) noexcept
      : category_{category}, code_{code}, message_{std::move(message)} {
  }

  ErrorCategory category_{ErrorCategory::errnum};
  int code_{};
  std::string message_;
};

template <>
struct fmt::formatter<Error> : formatter<string_view> {
  constexpr auto format(const Error& val, format_context& ctx)
      -> format_context::iterator {
    return formatter<string_view>::format(val.to_string(), ctx);
  }
};
