#ifndef KERO_STATUS_H_
#define KERO_STATUS_H_

#include <source_location>
#include <sstream>
#include <system_error>

namespace kero {

enum class ErrorCode : int {
  kBegin = 20000,
  kUnexpectedError,
  kEnd,
};

}  // namespace kero

constexpr std::ostream& operator<<(std::ostream& os,
                                   kero::ErrorCode val) noexcept {
  switch (val) {
    case kero::ErrorCode::kBegin:
      return os << "kero::ErrorCode::kBegin";
    case kero::ErrorCode::kUnexpectedError:
      return os << "kero::ErrorCode::kUnexpectedError";
    case kero::ErrorCode::kEnd:
      return os << "kero::ErrorCode::kEnd";
  }

  return os;
}

template <>
struct std::is_error_code_enum<kero::ErrorCode> : std::true_type {};

template <>
struct std::is_error_code_enum<std::errc> : std::true_type {};

namespace kero {

constexpr bool IsErrorCode(int code) noexcept {
  return static_cast<int>(ErrorCode::kBegin) <= code &&
         code <= static_cast<int>(ErrorCode::kEnd);
}

class Category : public std::error_category {
 public:
  const char* name() const noexcept override { return "kero"; }

  [[nodiscard]] std::string message(int condition) const noexcept override {
    std::stringstream ss;
    if (IsErrorCode(condition)) {
      ss << static_cast<ErrorCode>(condition);
    } else {
      ss << "Unknown error " << condition;
    }
    return ss.str();
  }

  static inline const Category& Get() noexcept {
    static Category instance;
    return instance;
  }
};

// NOTE: std::error_code constructor overload
inline std::error_code make_error_code(kero::ErrorCode e) noexcept {
  return std::error_code{static_cast<int>(e), kero::Category::Get()};
}

inline std::error_code CreateErrorCode(int errnum) noexcept {
  return std::error_code{errnum, std::system_category()};
}

class Status {
 public:
  Status() noexcept = default;

  inline Status(ErrorCode e, std::source_location location =
                                 std::source_location::current()) noexcept
      : code_{e}, location_{location} {}
  inline Status(std::errc e, std::source_location location =
                                 std::source_location::current()) noexcept
      : code_{e}, location_{location} {}
  inline Status(int e, std::source_location location =
                           std::source_location::current()) noexcept
      : code_{CreateErrorCode(e)}, location_{location} {}

  Status(const Status&) noexcept = default;
  Status(Status&&) noexcept = default;

  ~Status() noexcept = default;

  Status& operator=(const Status&) noexcept = default;
  Status& operator=(Status&&) noexcept = default;

  inline explicit operator bool() const noexcept {
    return code_.operator bool();
  }

  inline std::error_code Code() const noexcept { return code_; }
  constexpr std::source_location Location() const noexcept { return location_; }

 private:
  std::error_code code_;
  std::source_location location_;
};

}  // namespace kero

#endif  // KERO_STATUS_H_