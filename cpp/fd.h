#pragma once

using RawFd = int;

class Fd {
public:
  using self_type = Fd;
  using value_type = RawFd;

  Fd() noexcept = default;
  Fd(const self_type&) = delete;

  constexpr Fd(self_type&& other) noexcept : raw_fd_{other.raw_fd_} {
    other.raw_fd_ = invalid_raw_fd;
  }

  inline ~Fd() noexcept {
    reset();
  }

  self_type& operator=(const self_type&) = delete;

  constexpr self_type&
  operator=(self_type&& other) noexcept {
    if (this != &other) {
      reset();
      raw_fd_ = other.raw_fd_;
      other.raw_fd_ = invalid_raw_fd;
    }
    return *this;
  }

  constexpr value_type
  operator*() const noexcept {
    return value();
  }

  constexpr explicit
  operator bool() const noexcept {
    return has_value();
  }

  constexpr bool
  has_value() const noexcept {
    return is_valid_raw_fd();
  }

  constexpr value_type
  value() const noexcept {
    return as_raw_fd();
  }

  constexpr void
  reset() noexcept {
    close();
  }

  void close() noexcept;

  constexpr bool
  is_valid_raw_fd() const noexcept {
    return raw_fd_ != invalid_raw_fd;
  }

  constexpr value_type
  as_raw_fd() const noexcept {
    return raw_fd_;
  }

  static constexpr value_type invalid_raw_fd = -1;

private:
  value_type raw_fd_{invalid_raw_fd};
};
