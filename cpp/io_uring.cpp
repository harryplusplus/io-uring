#include "io_uring.h"

#include "error.h"
#include "event_fd.h"

IoUring::IoUring(IoUring&& other) noexcept : ring_{other.ring_} {
  other.ring_ = create_invalid_ring();
}

IoUring::~IoUring() noexcept {
  queue_exit();
}

auto IoUring::operator=(IoUring&& other) noexcept -> IoUring& {
  if (this != &other) {
    queue_exit();
    ring_ = other.ring_;
    other.ring_ = create_invalid_ring();
  }

  return *this;
}

auto IoUring::queue_exit() noexcept -> void {
  if (is_invalid_ring(ring_))
    return;

  struct io_uring ring = ring_;
  ring_ = create_invalid_ring();
  io_uring_queue_exit(&ring);
}

auto IoUring::register_eventfd_async(const EventFd& event_fd) noexcept
    -> Result<void> {
  if (is_invalid_ring(ring_))
    return err(
        {kero_errc::invalid_ring,
         fmt::format("Failed to call register_eventfd_async. event_fd: {}",
                     event_fd)});

  const int ret =
      io_uring_register_eventfd_async(&ring_, event_fd.as_fd().as_raw_fd());
  if (ret < 0)
    return err(
        {create_generic_error_code(-ret),
         fmt::format(
             "Failed to call io_uring_register_eventfd_async. event_fd: {}",
             event_fd)});

  return {};
}

auto IoUring::unregister_eventfd() noexcept -> Result<void> {
  if (is_invalid_ring(ring_))
    return err({kero_errc::invalid_ring, "Failed to call unregister_eventfd."});

  const int ret = io_uring_unregister_eventfd(&ring_);
  if (ret < 0)
    return err({create_generic_error_code(-ret),
                "Failed to call io_uring_unregister_eventfd."});

  return {};
}

auto IoUring::queue_init(unsigned int entries, unsigned int flags) noexcept
    -> Result<IoUring> {
  struct io_uring ring {};
  const int ret = io_uring_queue_init(entries, &ring, flags);
  if (ret < 0) {
    return err(
        {create_generic_error_code(-ret),
         fmt::format(
             "Failed to call io_uring_queue_init. entries: {}, flags: {}",
             entries, flags)});
  }

  return IoUring{ring};
}

IoUring::IoUring(struct io_uring ring) noexcept : ring_{ring} {
}
