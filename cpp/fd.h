#pragma once

#include "result.h"

class Fd {
public:
  Fd(const Fd&) = delete;
  Fd(Fd&&) noexcept;
  ~Fd() noexcept;

  auto operator=(const Fd&) -> Fd& = delete;
  auto operator=(Fd&&) noexcept -> Fd&;

  auto close() noexcept -> Result<void>;

  static auto open(std::string_view pathname, int flags, mode_t mode) noexcept
      -> Result<Fd>;

  static auto from_raw_fd(int raw_fd) noexcept -> Result<Fd>;

  static constexpr int invalid_raw_fd = -1;

private:
  Fd(int raw_fd) noexcept;

  int raw_fd_{invalid_raw_fd};
};
