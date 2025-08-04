#include "create.h"

#include <sys/epoll.h>
#include <sys/eventfd.h>

#include "fd.h"

using namespace kero;

Result<Fd, Error> create_event_fd() noexcept {
  const int ret = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if (ret == -1) return err(errno).reason("Failed to eventfd.");

  return Fd::from_raw_fd(ret);
}

Result<Fd, Error> create_epoll_fd() noexcept {
  const int ret = ::epoll_create1(EPOLL_CLOEXEC);
  if (ret == -1) return err(errno).reason("Failed to epoll_create1.");

  return Fd::from_raw_fd(ret);
}

Result<void, Error> add_event_fd_to_epoll(const Fd& epoll_fd,
                                          const Fd& event_fd) noexcept {
  if (!epoll_fd)
    return err(std::errc::invalid_argument).reason("Epoll fd must valid.");

  if (!event_fd)
    return err(std::errc::invalid_argument).reason("Event fd must valid.");

  struct epoll_event ev{.events = EPOLLIN | EPOLLET, .data = {.fd = *event_fd}};
  if (int ret = epoll_ctl(*epoll_fd, EPOLL_CTL_ADD, *event_fd, &ev); ret == -1)
    return err(errno)
        .detail("epoll_fd", epoll_fd)
        .detail("event_fd", event_fd)
        .reason("Failed to epoll_ctl to add event fd.");

  return {};
}

Result<CreateOutput, Error> create(CreateInput&& input) noexcept {
  if (auto res = create_event_fd(); !res) {
    return err(Errc::unexpected_error, std::move(res).error().into_cause())
        .reason("Failed to create event fd for Stopper.");
  }

  else {
    Fd stop_event_fd{*std::move(res)};

    if (auto res = create_epoll_fd(); !res) {
      return err(Errc::unexpected_error, std::move(res).error().into_cause())
          .reason("Failed to create epoll fd.");
    }

    else {
      Fd epoll_fd{*std::move(res)};

      if (auto res = add_event_fd_to_epoll(epoll_fd, stop_event_fd); !res) {
        return err(kero::Errc::unexpected_error,
                   std::move(res).error().into_cause())
            .reason("Failed to add stop event fd to epoll.");
      }

      std::shared_ptr<Fd> stop_event_fd_ptr{
          std::make_shared<Fd>(std::move(stop_event_fd))};
    }
  }
}
