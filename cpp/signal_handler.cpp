#include "signal_handler.h"

void
SignalHandler::init() noexcept {
  if (is_initialized)
    return;
  is_initialized = true;

  struct sigaction sa {};
  sa.sa_handler = on_signal;

  for (auto signo : shutdown_signals)
    sigaction(signo, &sa, nullptr);
}

void
SignalHandler::on_signal(int signal) noexcept {
  if (shutdown_flag)
    return;

  for (auto shutdown_signal : shutdown_signals)
    if (signal == shutdown_signal) {
      shutdown_flag = 1;
      break;
    }
}

bool SignalHandler::is_initialized = false;
volatile sig_atomic_t SignalHandler::shutdown_flag = 0;
