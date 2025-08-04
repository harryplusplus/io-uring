#ifndef KERO_FD_H_
#define KERO_FD_H_

#include "error.h"
#include "result.h"

namespace kero {

using RawFd = int;

class Fd {
 public:
  Fd() noexcept = default;
  Fd(const Fd&) = delete;

  constexpr Fd(Fd&& other) noexcept : raw_fd_{other.raw_fd_} {
    other.raw_fd_ = invalid_raw_fd;
  }

  inline ~Fd() noexcept { reset(); }

  Fd& operator=(const Fd&) = delete;

  constexpr Fd& operator=(Fd&& other) noexcept {
    if (this != &other) {
      reset();
      raw_fd_ = other.raw_fd_;
      other.raw_fd_ = invalid_raw_fd;
    }
    return *this;
  }

  constexpr RawFd operator*() const noexcept { return value(); }

  constexpr explicit operator bool() const noexcept { return has_value(); }

  constexpr bool has_value() const noexcept { return is_valid_raw_fd(); }

  constexpr RawFd value() const noexcept { return as_raw_fd(); }

  constexpr void reset() noexcept { close(); }

  void close() noexcept;

  constexpr bool is_valid_raw_fd() const noexcept {
    return raw_fd_ != invalid_raw_fd;
  }

  constexpr RawFd as_raw_fd() const noexcept { return raw_fd_; }

  static inline Result<Fd, Error> from_raw_fd(RawFd raw_fd) noexcept {
    if (raw_fd < 0)
      return err(std::errc::invalid_argument)
          .detail("raw_fd", raw_fd)
          .reason("raw_fd is less than 0.");

    return Fd{raw_fd};
  }

  static constexpr RawFd invalid_raw_fd = -1;

 private:
  constexpr Fd(RawFd raw_fd) noexcept : raw_fd_{raw_fd} {}

  RawFd raw_fd_ = invalid_raw_fd;
};

}  // namespace kero

namespace std {

inline ostream& operator<<(ostream& os, const kero::Fd& val) noexcept {
  os << "Fd{";

  if (val) os << "raw_fd:" << *val;

  os << "}";
  return os;
}

}  // namespace std

#endif  // KERO_FD_H_
