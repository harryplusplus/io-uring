#pragma once

#include <fmt/core.h>
#include <fmt/format.h>
#include <string>
#include <vector>

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

class ErrnumError {
public:
  constexpr ErrnumError(int code, std::string message) noexcept
      : code{code}, message{std::move(message)} {
  }

  ErrnumError(const ErrnumError&) = delete;
  ErrnumError(ErrnumError&&) noexcept = default;

  ~ErrnumError() noexcept = default;

  ErrnumError& operator=(const ErrnumError&) = delete;
  ErrnumError& operator=(ErrnumError&&) noexcept = default;

  int code;
  std::string message;
};

enum class KeroErrorCode : int {
  unexpected_error = 1000,
};

constexpr std::string_view
to_name(KeroErrorCode code) noexcept {
  switch (code) {
  case KeroErrorCode::unexpected_error:
    return "unexpected_error";
  }
}

class KeroError {
public:
  constexpr KeroError(KeroErrorCode code, std::string message) noexcept
      : code{code}, message{std::move(message)} {
  }

  KeroError(const KeroError&) = delete;
  KeroError(KeroError&&) noexcept = default;

  ~KeroError() noexcept = default;

  KeroError& operator=(const KeroError&) = delete;
  KeroError& operator=(KeroError&&) noexcept = default;

  KeroErrorCode code;
  std::string message;
};

class Error {
public:
  Error() noexcept = default;

  constexpr Error(ErrnumError&& err) noexcept
      : category_{ErrorCategory::errnum}, code_{err.code}, message_{std::move(
                                                               err.message)} {
  }

  constexpr Error(KeroError&& err) noexcept
      : category_{ErrorCategory::kero}, code_{static_cast<int>(err.code)},
        message_{std::move(err.message)} {
  }

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
    using Attr = std::pair<std::string_view, std::string>;

    std::vector<Attr> attrs{
        Attr{"category", ::to_string(category_)},
        Attr{"code", std::to_string(code_)},
    };

    switch (category_) {
    case ErrorCategory::undefined:
      break;
    case ErrorCategory::errnum: {
      std::string_view name = errnum_to_name(code_);
      if (!name.empty())
        attrs.push_back(Attr{"name", name});

      std::string_view desc = errnum_to_desc(code_);
      if (!desc.empty())
        attrs.push_back(Attr{"desc", desc});
    } break;
    case ErrorCategory::kero: {
      attrs.push_back(Attr{"name", to_name(static_cast<KeroErrorCode>(code_))});
    } break;
    }

    if (!message_.empty())
      attrs.push_back(Attr{"message", message_});

    std::string ret{"Error{"};
    for (auto it = attrs.begin(); it != attrs.end(); it++) {
      auto [key, val] = *it;
      ret += fmt::format("{}:{}", key, val);
      if (std::next(it) != attrs.end())
        ret += ",";
    }

    ret += "}";
    return ret;
  }

private:
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
