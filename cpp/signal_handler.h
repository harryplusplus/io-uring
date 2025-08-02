#pragma once

#include <array>
#include <signal.h>

class SignalHandler {
public:
  static void init() noexcept;

private:
  static void on_signal(int signal) noexcept;

  static bool is_initialized;
  static constexpr std::array<int, 2> shutdown_signals{SIGINT, SIGTERM};
  static volatile sig_atomic_t shutdown_flag;
};
