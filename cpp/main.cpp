#include <iostream>
#include <liburing.h>
#include <string.h>

int main() {
    std::cout << "Hello, World!" << std::endl;
    struct io_uring ring{};
    int ret = io_uring_queue_init(8, &ring, 0);
    if (ret < 0) {
        std::cerr << "io_uring_queue_init failed. code: " << ret << ", " << strerror(-ret) << std::endl;
        return 1;
    }
    std::cout << "io_uring_queue_init succeeded" << std::endl;
    io_uring_queue_exit(&ring);
    return 0;
}
