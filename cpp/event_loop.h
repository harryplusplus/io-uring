#pragma once

#include "error.h"
#include "fd.h"
#include "result.h"

namespace kero {

class Runner {};

class Stopper {};

class EventLoop {
public:
  struct Config {
    uint io_uring_queue_entries{1024};
  };

  EventLoop() noexcept = default;
  EventLoop(const EventLoop&) = delete;
  EventLoop(EventLoop&&) noexcept = default;

  EventLoop& operator=(const EventLoop&) = delete;
  EventLoop& operator=(EventLoop&&) noexcept = default;

  ~EventLoop() noexcept;

  [[nodiscard]] Result<void, Error> run(const Config& config) noexcept;

  [[nodiscard]] static Result<std::tuple<Runner, Stopper>, Error>
  create() noexcept;
};

} // namespace kero
