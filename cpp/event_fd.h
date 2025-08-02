#pragma once

#include <fmt/format.h>

#include "fd.h"

class EventFd {
public:
  EventFd(const EventFd&) = delete;
  EventFd(EventFd&&) noexcept = default;

  ~EventFd() noexcept = default;

  auto operator=(const EventFd&) -> EventFd& = delete;
  auto operator=(EventFd&&) noexcept -> EventFd& = default;

  constexpr auto as_fd() const noexcept -> const Fd& {
    return fd_;
  }

  constexpr auto to_string() const noexcept -> std::string {
    return fmt::format("EventFd{{fd:{}}}", fd_);
  }

  static auto create(unsigned int initval, int flags) noexcept
      -> Result<EventFd>;

private:
  EventFd(Fd&& fd) noexcept;

  Fd fd_;
};

template <>
struct fmt::formatter<EventFd> : formatter<string_view> {
  constexpr auto format(const EventFd& val, format_context& ctx)
      -> format_context::iterator {
    return formatter<string_view>::format(val.to_string(), ctx);
  }
};
