#ifndef KERO_STOPPER_H_
#define KERO_STOPPER_H_

#include "fd.h"

namespace kero {

class Stopper {
 public:
  inline Stopper(std::weak_ptr<Fd>&& event_fd) noexcept
      : event_fd_{std::move(event_fd)} {
    assert(!event_fd_.expired());
  }

  Stopper(const Stopper&) = delete;
  Stopper(Stopper&&) noexcept = default;

  ~Stopper() noexcept = default;

  Stopper& operator=(const Stopper&) = delete;
  Stopper& operator=(Stopper&&) noexcept = default;

  Result<void, Error> stop() const noexcept;

 private:
  std::weak_ptr<Fd> event_fd_;
};

}  // namespace kero

#endif  // KERO_STOPPER_H_
