#pragma once

#include <sstream>
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

constexpr ostream&
operator<<(ostream& os, kero::Errc val) noexcept {
  switch (val) {
  case kero::Errc::unexpected_error:
    return os << "unexpected_error";
  }
}

template <>
struct is_error_code_enum<kero::Errc> : true_type {};

} // namespace std

namespace kero {

class Category : public std::error_category {
public:
  inline virtual const char*
  name() const noexcept override {
    return "kero";
  }

  [[nodiscard]] inline virtual std::string
  message(int condition) const noexcept override {
    std::stringstream ss;
    if (is_errc(condition)) {
      ss << static_cast<Errc>(condition);
    } else {
      ss << "Unknown error " << condition;
    }
    return ss.str();
  }
};

inline const Category&
category() noexcept {
  static Category instance;
  return instance;
}

} // namespace kero

namespace std {

[[nodiscard]] inline error_code
make_error_code(kero::Errc e) noexcept {
  return error_code{static_cast<int>(e), kero::category()};
}

} // namespace std

namespace kero {

class Error {
public:
  class Builder {
  public:
    inline Builder(std::error_code&& ec) noexcept : error_code_{std::move(ec)} {
    }

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
    detail(std::string&& key, std::string&& val) && noexcept {
      details_.insert({std::move(key), std::move(val)});
      return std::move(*this);
    }

    template <typename T>
    inline Builder&&
    detail(std::string&& key, T&& val) && noexcept {
      std::stringstream ss;
      ss << std::forward<T&&>(val);
      details_.insert({std::move(key), ss.str()});
      return std::move(*this);
    }

    [[nodiscard]] inline Error
    build() && noexcept {
      return Error{std::move(*this)};
    }

  private:
    std::error_code error_code_;
    std::string reason_;
    std::unordered_map<std::string, std::string> details_;

    friend Error;
  };

  inline Error(Builder&& builder) noexcept
      : error_code_{std::move(builder.error_code_)},
        reason_{std::move(builder.reason_)},
        details_{std::move(builder.details_)} {
  }

  Error(const Error&) = delete;
  Error(Error&&) noexcept = default;

  ~Error() noexcept = default;

  Error& operator=(const Error&) = delete;
  Error& operator=(Error&&) noexcept = default;

  inline const std::error_code
  error_code() const noexcept {
    return error_code_;
  }

  constexpr const std::string&
  reason() const noexcept {
    return reason_;
  }

  inline const std::unordered_map<std::string, std::string>
  details() const noexcept {
    return details_;
  }

private:
  std::error_code error_code_;
  std::string reason_;
  std::unordered_map<std::string, std::string> details_;
};

[[nodiscard]] inline Error::Builder
err(int errnum) noexcept {
  return Error::Builder{std::error_code{errnum, std::system_category()}};
}

[[nodiscard]] inline Error::Builder
err(Errc e) noexcept {
  return Error::Builder{std::make_error_code(e)};
}

} // namespace kero

namespace std {

inline ostream&
operator<<(ostream& os, const kero::Error& val) noexcept {
  os << "Error{";
  os << "error_code:" << val.error_code() << ",";
  os << "reason:" << val.reason() << ",";
  os << "details:{";
  auto&& details = val.details();
  for (auto it = details.begin(); it != details.end(); it++) {
    os << it->first << ":" << it->second;
    if (std::next(it) != details.end())
      os << ",";
  }
  os << "}";
  os << "}";
  return os;
}

} // namespace std
