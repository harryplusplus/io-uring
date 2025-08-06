#ifndef KERO_CONFIG_H_
#define KERO_CONFIG_H_

#include <sys/types.h>

namespace kero {

struct Config {
  uint io_uring_queue_entries = 1024;
  size_t epoll_events_size = 1024;
};

}  // namespace kero

#endif  // KERO_CONFIG_H_
