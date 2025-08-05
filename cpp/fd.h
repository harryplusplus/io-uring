#ifndef KERO_FD_H_
#define KERO_FD_H_

#include <cassert>
#include <ostream>

namespace kero {

using RawFd = int;

class Fd {
 public:
  Fd() noexcept = default;

  constexpr Fd(RawFd raw_fd) noexcept : raw_fd_{raw_fd} {
    assert(raw_fd_ > kInvalidRawFd);
  }

  Fd(const Fd&) = delete;

  constexpr Fd(Fd&& other) noexcept : raw_fd_{other.raw_fd_} {
    other.raw_fd_ = kInvalidRawFd;
  }

  inline ~Fd() noexcept { Close(); }

  Fd& operator=(const Fd&) = delete;

  constexpr Fd& operator=(Fd&& other) noexcept {
    if (this != &other) {
      this->~Fd();
      raw_fd_ = other.raw_fd_;
      other.raw_fd_ = kInvalidRawFd;
    }
    return *this;
  }

  constexpr RawFd operator*() const noexcept { return Value(); }

  constexpr explicit operator bool() const noexcept { return HasValue(); }

  constexpr bool HasValue() const noexcept { return raw_fd_ != kInvalidRawFd; }

  constexpr RawFd Value() const noexcept { return raw_fd_; }

  void Close() noexcept;

  static constexpr RawFd kInvalidRawFd = -1;

 private:
  RawFd raw_fd_ = kInvalidRawFd;
};

}  // namespace kero

inline std::ostream& operator<<(std::ostream& os,
                                const kero::Fd& val) noexcept {
  os << "Fd{";

  if (val) os << "raw_fd:" << *val;

  os << "}";
  return os;
}

#endif  // KERO_FD_H_
