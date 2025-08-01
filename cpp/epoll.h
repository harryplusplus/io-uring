#pragma once

#include "expected.h"

class Epoll {
public:
  ~Epoll() noexcept;

  [[nodiscard]] auto close() noexcept -> Expected<void>;

  [[nodiscard]] static auto create1(int flags) noexcept -> Expected<Epoll>;

private:
  explicit Epoll(int fd) noexcept;

  static constexpr int invalid_fd = -1;
  int fd_{invalid_fd};
};
