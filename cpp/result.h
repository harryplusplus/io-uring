#ifndef KERO_RESULT_H_
#define KERO_RESULT_H_

#include <cassert>
#include <variant>

namespace kero {

template <typename T, typename E>
class Result {
 public:
  using value_type = T;
  using error_type = E;

  constexpr Result(value_type&& val) noexcept : data_{std::move(val)} {}

  constexpr Result(error_type&& err) noexcept : data_{std::move(err)} {}

  Result(const Result&) = delete;
  Result(Result&&) noexcept = default;

  ~Result() noexcept = default;

  Result& operator=(const Result&) = delete;
  Result& operator=(Result&&) noexcept = default;

  constexpr const value_type* operator->() const noexcept {
    assert(is_ok());
    return std::get_if<value_type>(&data_);
  }

  constexpr value_type* operator->() noexcept {
    assert(is_ok());
    return std::get_if<value_type>(&data_);
  }

  constexpr const value_type& operator*() const& noexcept { return value(); }

  constexpr value_type& operator*() & noexcept { return value(); }

  constexpr const value_type&& operator*() const&& noexcept { return value(); }

  constexpr value_type&& operator*() && noexcept { return value(); }

  constexpr explicit operator bool() const noexcept { return is_ok(); }

  constexpr bool has_value() const noexcept { return is_ok(); }

  constexpr bool is_ok() const noexcept {
    return std::holds_alternative<value_type>(data_);
  }

  constexpr bool is_err() const noexcept {
    return std::holds_alternative<error_type>(data_);
  }

  constexpr value_type& value() & noexcept {
    assert(is_ok());
    return std::get<value_type>(data_);
  }

  constexpr const value_type& value() const& noexcept {
    assert(is_ok());
    return std::get<value_type>(data_);
  }

  constexpr value_type&& value() && noexcept {
    assert(is_ok());
    return std::get<value_type>(std::move(data_));
  }

  constexpr const value_type&& value() const&& noexcept {
    assert(is_ok());
    return std::get<value_type>(std::move(data_));
  }

  constexpr error_type& error() & noexcept {
    assert(is_err());
    return std::get<error_type>(data_);
  }

  constexpr const error_type& error() const& noexcept {
    assert(is_err());
    return std::get<error_type>(data_);
  }

  constexpr error_type&& error() && noexcept {
    assert(is_err());
    return std::get<error_type>(std::move(data_));
  }

  constexpr const error_type&& error() const&& noexcept {
    assert(is_err());
    return std::get<error_type>(std::move(data_));
  }

 private:
  std::variant<T, E> data_;
};

template <typename E>
class Result<void, E> {
 public:
  using value_type = void;
  using error_type = E;

  constexpr Result() noexcept : data_{std::monostate{}} {}

  constexpr Result(error_type&& err) noexcept : data_{std::move(err)} {}

  Result(const Result&) = delete;
  Result(Result&&) noexcept = default;

  ~Result() noexcept = default;

  Result& operator=(const Result&) = delete;
  Result& operator=(Result&&) noexcept = default;

  constexpr explicit operator bool() const noexcept { return is_ok(); }

  constexpr bool is_ok() const noexcept {
    return std::holds_alternative<std::monostate>(data_);
  }

  constexpr bool is_err() const noexcept {
    return std::holds_alternative<error_type>(data_);
  }

  constexpr error_type& error() & noexcept {
    assert(is_err());
    return std::get<error_type>(data_);
  }

  constexpr const error_type& error() const& noexcept {
    assert(is_err());
    return std::get<error_type>(data_);
  }

  constexpr error_type&& error() && noexcept {
    assert(is_err());
    return std::get<error_type>(data_);
  }

  constexpr const error_type&& error() const&& noexcept {
    assert(is_err());
    return std::get<error_type>(data_);
  }

 private:
  std::variant<std::monostate, E> data_;
};

}  // namespace kero

#endif  // KERO_RESULT_H_
