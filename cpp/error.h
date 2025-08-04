#ifndef KERO_ERROR_H_
#define KERO_ERROR_H_

#include <cassert>
#include <memory>
#include <sstream>
#include <string>
#include <system_error>
#include <unordered_map>

namespace kero {

enum class Errc : int {
  _begin = 1000,
  unexpected_error,
  _end,
};

constexpr bool is_errc(int code) noexcept {
  return static_cast<int>(Errc::_begin) <= code &&
         code <= static_cast<int>(Errc::_end);
}

}  // namespace kero

namespace std {

constexpr ostream& operator<<(ostream& os, kero::Errc val) noexcept {
  switch (val) {
    case kero::Errc::_begin:
      return os << "kero::Errc::_begin";
    case kero::Errc::unexpected_error:
      return os << "kero::Errc::unexpected_error";
    case kero::Errc::_end:
      return os << "kero::Errc::_end";
  }
}

template <>
struct is_error_code_enum<kero::Errc> : true_type {};

}  // namespace std

namespace kero {

class Category : public std::error_category {
 public:
  const char* name() const noexcept override { return "kero"; }

  [[nodiscard]] std::string message(int condition) const noexcept override {
    std::stringstream ss;
    if (is_errc(condition)) {
      ss << static_cast<Errc>(condition);
    } else {
      ss << "Unknown error " << condition;
    }
    return ss.str();
  }

  static inline const Category& get() noexcept {
    static Category instance;
    return instance;
  }
};

}  // namespace kero

namespace std {

[[nodiscard]] inline error_code make_error_code(kero::Errc e) noexcept {
  return error_code{static_cast<int>(e), kero::Category::get()};
}

}  // namespace std

namespace kero {

class Error {
 public:
  using Details = std::unordered_map<std::string, std::string>;

  struct Data {
    Details details;
    std::string reason;
    std::error_code error_code;
    std::unique_ptr<Error> cause;

    static_assert(sizeof(details) >= sizeof(reason));
    static_assert(sizeof(reason) >= sizeof(error_code));
    static_assert(sizeof(error_code) >= sizeof(cause));
  };

  class Builder {
   public:
    inline Builder(std::unique_ptr<Data>&& data) noexcept
        : data_{std::move(data)} {
      check();
    }

    Builder(const Builder&) = delete;
    Builder(Builder&&) = delete;

    ~Builder() noexcept = default;

    Builder& operator=(const Builder&) = delete;
    Builder& operator=(Builder&&) = delete;

    constexpr Builder&& reason(std::string&& reason) && noexcept {
      check();
      data_->reason = std::move(reason);
      return std::move(*this);
    }

    constexpr Builder&& detail(std::string&& key,
                               std::string&& val) && noexcept {
      check();
      data_->details.insert({std::move(key), std::move(val)});
      return std::move(*this);
    }

    template <typename T>
    Builder&& detail(std::string&& key, T&& val) && noexcept {
      check();
      std::stringstream ss;
      ss << std::forward<T&&>(val);
      data_->details.insert({std::move(key), ss.str()});
      return std::move(*this);
    }

    [[nodiscard]] inline Error build() && noexcept {
      check();
      return Error{std::move(this->data_)};
    }

   private:
    constexpr void check() const noexcept { assert(data_); }

    std::unique_ptr<Data> data_;
  };

  inline Error(std::unique_ptr<Data>&& data) noexcept : data_{std::move(data)} {
    check();
  }

  Error(const Error&) = delete;
  Error(Error&&) noexcept = default;

  ~Error() noexcept = default;

  Error& operator=(const Error&) = delete;
  Error& operator=(Error&&) noexcept = default;

  inline const std::error_code& error_code() const& noexcept {
    check();
    return data_->error_code;
  }

  constexpr const std::string& reason() const& noexcept {
    check();
    return data_->reason;
  }

  inline const Details& details() const& noexcept {
    check();
    return data_->details;
  }

  inline const std::unique_ptr<Error>& cause() const& noexcept {
    check();
    return data_->cause;
  }

  inline std::unique_ptr<Error> into_cause() && noexcept {
    check();
    return std::make_unique<Error>(std::move(*this));
  }

 private:
  constexpr void check() const noexcept { assert(data_); }

  std::unique_ptr<Data> data_;
};

template <typename T>
[[nodiscard]] inline Error::Builder err(
    T e, std::unique_ptr<Error>&& cause = {}) noexcept {
  return Error::Builder{std::make_unique<Error::Data>(Error::Data{
      .error_code = std::make_error_code(e),
      .cause = std::move(cause),
  })};
}

template <>
[[nodiscard]] inline Error::Builder err<int>(
    int errnum, std::unique_ptr<Error>&& cause) noexcept {
  return Error::Builder{std::make_unique<Error::Data>(Error::Data{
      .error_code = std::error_code{errnum, std::system_category()},
      .cause = std::move(cause),
  })};
}

}  // namespace kero

namespace std {

inline ostream& operator<<(ostream& os, const kero::Error& val) noexcept {
  os << "Error{";
  os << "error_code:" << val.error_code();

  const auto& reason = val.reason();
  if (!reason.empty()) os << ",reason:" << reason;

  const auto& details = val.details();
  if (!details.empty()) {
    os << ",details:{";
    for (auto it = details.begin(); it != details.end(); it++) {
      os << it->first << ":" << it->second;
      if (std::next(it) != details.end()) os << ",";
    }
    os << "}";
  }

  const auto& cause = val.cause();
  if (cause) os << ",cause:" << cause;

  os << "}";
  return os;
}

}  // namespace std

#endif  // KERO_ERROR_H_
