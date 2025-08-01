#pragma once

#include "fd.h"
#include "result.h"

class Epoll {
public:
  Epoll(const Epoll&) = delete;
  Epoll(Epoll&&) noexcept = default;
  ~Epoll() noexcept;

  auto operator=(const Epoll&) -> Epoll& = delete;
  auto operator=(Epoll&&) noexcept -> Epoll& = default;

  [[nodiscard]] auto close() noexcept -> Result<void>;

  [[nodiscard]] static auto create1(int flags) noexcept -> Result<Epoll>;

private:
  Epoll(Fd&& fd) noexcept;

  Fd fd_;
};
