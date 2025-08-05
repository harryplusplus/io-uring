#ifndef KERO_TAG_H_
#define KERO_TAG_H_

template <typename T, typename Id>
class Tag {
 public:
  using ValueType = T;
  using IdType = Id;

  constexpr Tag(const T& value) noexcept : value_{value} {}
  constexpr Tag(T&& value) noexcept : value_{std::move(value)} {}

  Tag(const Tag&) noexcept = default;
  Tag(Tag&&) noexcept = default;

  ~Tag() noexcept = default;

  Tag& operator=(const Tag&) noexcept = default;
  Tag& operator=(Tag&&) noexcept = default;

  constexpr ValueType* operator->() noexcept { return &value_; }
  constexpr const ValueType* operator->() const noexcept { return &value_; }

  constexpr ValueType& operator*() & noexcept { return Value(); }
  constexpr const ValueType& operator*() const& noexcept { return Value(); }
  constexpr ValueType&& operator*() && noexcept { return std::move(Value()); }
  constexpr const ValueType&& operator*() const&& noexcept {
    return std::move(Value());
  }

  constexpr T& Value() & noexcept { return value_; }
  constexpr const T& Value() const& noexcept { return value_; }
  constexpr T&& Value() && noexcept { return std::move(value_); }
  constexpr const T&& Value() const&& noexcept { return std::move(value_); }

 private:
  T value_;
};

#endif  // KERO_TAG_H_