// #include "fd.h"

// #include <unistd.h>

// Fd::Fd(Fd&& other) noexcept : raw_fd_{other.raw_fd_} {
//   other.raw_fd_ = invalid_raw_fd;
// }

// Fd::~Fd() noexcept {
//   [[maybe_unused]] auto res = close();
// }

// auto
// Fd::operator=(Fd&& other) noexcept -> Fd& {
//   if (this != &other) {
//     [[maybe_unused]] auto res = close();
//     raw_fd_ = other.raw_fd_;
//     other.raw_fd_ = invalid_raw_fd;
//   }

//   return *this;
// }

// auto
// Fd::close() noexcept -> Result<void> {
//   if (!is_valid())
//     return {};

//   const RawFd raw_fd = raw_fd_;
//   raw_fd_ = invalid_raw_fd;

//   const int ret = ::close(raw_fd);
//   if (ret == -1)
//     return err(Error::errnum(
//         errno, fmt::format("Failed to call close. raw_fd: {}", raw_fd)));

//   return {};
// }

// auto
// Fd::from_raw_fd(RawFd raw_fd) noexcept -> Result<Fd> {
//   if (raw_fd < 0)
//     return err(
//         Error::kero(KeroErrorCode::invalid_fd,
//                     fmt::format("Failed to create Fd. raw_fd: {}", raw_fd)));

//   return Fd{raw_fd};
// }

// std::tuple<Fd, Error>
// Fd::from_raw_fd2(RawFd raw_fd) noexcept {
//   if (raw_fd < 0)
//     return {Fd{}, Error::kero(
//                       KeroErrorCode::invalid_fd,
//                       fmt::format("Failed to create Fd. raw_fd: {}", raw_fd))};

//   return {Fd{raw_fd}, Error{}};
// }

// Fd::Fd(RawFd raw_fd) noexcept : raw_fd_{raw_fd} {
// }
