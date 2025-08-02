#pragma once

#include <fmt/format.h>

#include "result.h"

using RawFd = int;

class Fd {
public:
  Fd() noexcept = default;
  Fd(const Fd&) = delete;
  Fd(Fd&&) noexcept;

  ~Fd() noexcept;

  auto operator=(const Fd&) -> Fd& = delete;
  auto operator=(Fd&&) noexcept -> Fd&;

  [[nodiscard]] auto close() noexcept -> Result<void>;

  constexpr auto is_valid() const noexcept -> bool {
    return raw_fd_ != invalid_raw_fd;
  }

  constexpr auto as_raw_fd() const noexcept -> RawFd {
    return raw_fd_;
  }

  [[nodiscard]] constexpr auto to_string() const noexcept -> std::string {
    return fmt::format("Fd{{raw_fd:{}}}", raw_fd_);
  }

  [[nodiscard]] static auto from_raw_fd(RawFd raw_fd) noexcept -> Result<Fd>;

  static constexpr RawFd invalid_raw_fd = -1;

private:
  Fd(RawFd raw_fd) noexcept;

  RawFd raw_fd_{invalid_raw_fd};
};

template <>
struct fmt::formatter<Fd> : formatter<string_view> {
  constexpr auto format(const Fd& val, format_context& ctx)
      -> format_context::iterator {
    return formatter<string_view>::format(val.to_string(), ctx);
  }
};
