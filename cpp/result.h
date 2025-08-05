#ifndef KERO_RESULT_H_
#define KERO_RESULT_H_

#include <cassert>
#include <variant>

namespace kero {

template <typename T, typename E>
class Result {
 public:
  using ValueType = T;
  using ErrorType = E;

  constexpr Result(ValueType&& val) noexcept : data_{std::move(val)} {}

  constexpr Result(ErrorType&& err) noexcept : data_{std::move(err)} {}

  Result(const Result&) = delete;
  Result(Result&&) noexcept = default;

  ~Result() noexcept = default;

  Result& operator=(const Result&) = delete;
  Result& operator=(Result&&) noexcept = default;

  constexpr const ValueType* operator->() const noexcept {
    assert(IsOk());
    return std::get_if<ValueType>(&data_);
  }

  constexpr ValueType* operator->() noexcept {
    assert(IsOk());
    return std::get_if<ValueType>(&data_);
  }

  constexpr const ValueType& operator*() const& noexcept { return Value(); }

  constexpr ValueType& operator*() & noexcept { return Value(); }

  constexpr const ValueType&& operator*() const&& noexcept {
    return std::move(Value());
  }

  constexpr ValueType&& operator*() && noexcept { return std::move(Value()); }

  constexpr explicit operator bool() const noexcept { return IsOk(); }

  constexpr bool HasValue() const noexcept { return IsOk(); }

  constexpr bool IsOk() const noexcept {
    return std::holds_alternative<ValueType>(data_);
  }

  constexpr bool IsErr() const noexcept {
    return std::holds_alternative<ErrorType>(data_);
  }

  constexpr ValueType& Value() & noexcept {
    assert(IsOk());
    return std::get<ValueType>(data_);
  }

  constexpr const ValueType& Value() const& noexcept {
    assert(IsOk());
    return std::get<ValueType>(data_);
  }

  constexpr ValueType&& Value() && noexcept {
    assert(IsOk());
    return std::move(std::get<ValueType>(data_));
  }

  constexpr const ValueType&& Value() const&& noexcept {
    assert(IsOk());
    return std::move(std::get<ValueType>(data_));
  }

  constexpr ErrorType& Error() & noexcept {
    assert(IsErr());
    return std::get<ErrorType>(data_);
  }

  constexpr const ErrorType& Error() const& noexcept {
    assert(IsErr());
    return std::get<ErrorType>(data_);
  }

  constexpr ErrorType&& Error() && noexcept {
    assert(IsErr());
    return std::move(std::get<ErrorType>(data_));
  }

  constexpr const ErrorType&& Error() const&& noexcept {
    assert(IsErr());
    return std::move(std::get<ErrorType>(data_));
  }

 private:
  std::variant<T, E> data_;
};

template <typename E>
class Result<void, E> {
 public:
  using ValueType = void;
  using ErrorType = E;

  constexpr Result() noexcept : data_{std::monostate{}} {}

  constexpr Result(ErrorType&& err) noexcept : data_{std::move(err)} {}

  Result(const Result&) = delete;
  Result(Result&&) noexcept = default;

  ~Result() noexcept = default;

  Result& operator=(const Result&) = delete;
  Result& operator=(Result&&) noexcept = default;

  constexpr explicit operator bool() const noexcept { return IsOk(); }

  constexpr bool IsOk() const noexcept {
    return std::holds_alternative<std::monostate>(data_);
  }

  constexpr bool IsErr() const noexcept {
    return std::holds_alternative<ErrorType>(data_);
  }

  constexpr ErrorType& Error() & noexcept {
    assert(IsErr());
    return std::get<ErrorType>(data_);
  }

  constexpr const ErrorType& Error() const& noexcept {
    assert(IsErr());
    return std::get<ErrorType>(data_);
  }

  constexpr ErrorType&& Error() && noexcept {
    assert(IsErr());
    return std::move(std::get<ErrorType>(data_));
  }

  constexpr const ErrorType&& Error() const&& noexcept {
    assert(IsErr());
    return std::move(std::get<ErrorType>(data_));
  }

 private:
  std::variant<std::monostate, E> data_;
};

}  // namespace kero

#endif  // KERO_RESULT_H_
