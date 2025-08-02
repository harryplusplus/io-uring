#pragma once

#include <fmt/format.h>
#include <sys/epoll.h>

#include "fd.h"

template <>
struct fmt::formatter<epoll_data_t> : formatter<string_view> {
  constexpr auto format(const epoll_data_t& val, format_context& ctx)
      -> format_context::iterator {
    return formatter<string_view>::format(
        fmt::format("epoll_data_t{{u64:{}}}", val.u64), ctx);
  }
};

template <>
struct fmt::formatter<epoll_event> : formatter<string_view> {
  constexpr auto format(const epoll_event& val, format_context& ctx)
      -> format_context::iterator {
    return formatter<string_view>::format(
        fmt::format("epoll_event{{events:{},data:{}}}", val.events, val.data),
        ctx);
  }
};

class Epoll {
public:
  Epoll(const Epoll&) = delete;
  Epoll(Epoll&&) noexcept = default;

  ~Epoll() noexcept = default;

  auto operator=(const Epoll&) -> Epoll& = delete;
  auto operator=(Epoll&&) noexcept -> Epoll& = default;

  auto ctl(int op, const Fd& fd, struct epoll_event& event) const noexcept
      -> Result<void>;

  auto pwait(struct epoll_event* events, int maxevents, int timeout,
             const sigset_t* sigmask) const noexcept -> Result<void>;

  [[nodiscard]] static auto create1(int flags) noexcept -> Result<Epoll>;

private:
  Epoll(Fd&& fd) noexcept;

  Fd fd_;
};
