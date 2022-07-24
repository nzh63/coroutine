/*
 * Copyright 2020-present nzh63
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <liburing.h>
#include <linux/io_uring.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdint>
#include <cstring>
#include <functional>
#include <iostream>

#include "Promise.h"
#include "Runtime.h"

namespace asyncio {
struct io_uring ring = {};

void init() {
    auto err = io_uring_queue_init(128, &ring, 0);
    if (err < 0) {
        std::cerr << "io_uring_queue_init error: " << strerror(-err)
                  << std::endl;
        exit(1);
    }
}

void looper() {
    while (true) {
        if (co::Runtime::instance()->yield()) continue;
        struct io_uring_cqe* cqe = nullptr;
        int err = io_uring_wait_cqe(&ring, &cqe);
        if (err < 0) {
            std::cerr << "io_uring_wait_cqe error: " << strerror(-err)
                      << std::endl;
            exit(1);
        }
        auto* resolver =
            reinterpret_cast<co::Promise<int>::Resolver*>(cqe->user_data);
        resolver->resolve(cqe->res);
        io_uring_cqe_seen(&ring, cqe);
    }
}

co::Promise<int> accept(int sockfd, struct sockaddr* addr, socklen_t* addrlen,
                        int flags) {
    return co::Promise<int>([&](co::Promise<int>::Resolver* resolver) {
        struct io_uring_sqe* sqe = io_uring_get_sqe(&ring);
        io_uring_prep_accept(sqe, sockfd, addr, addrlen, flags);
        io_uring_sqe_set_data(sqe, resolver);
        io_uring_submit(&ring);
    });
}

co::Promise<int> read(int fd, char* buffer, size_t buflen) {
    return co::Promise<int>([&](co::Promise<int>::Resolver* resolver) {
        struct io_uring_sqe* sqe = io_uring_get_sqe(&ring);
        io_uring_prep_read(sqe, fd, buffer, buflen, 0);
        io_uring_sqe_set_data(sqe, resolver);
        io_uring_submit(&ring);
    });
}

co::Promise<int> write(int fd, char* buffer, size_t buflen) {
    return co::Promise<int>([&](co::Promise<int>::Resolver* resolver) {
        struct io_uring_sqe* sqe = io_uring_get_sqe(&ring);
        io_uring_prep_write(sqe, fd, buffer, buflen, 0);
        io_uring_sqe_set_data(sqe, resolver);
        io_uring_submit(&ring);
    });
}
}  // namespace asyncio

int main() {
    auto* runtime = co::Runtime::instance();

    runtime->spawn([runtime]() {
        asyncio::init();
        asyncio::looper();
    });

    runtime->spawn([runtime]() {
        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        server_addr.sin_port = htons(8989);
        int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (listen_fd < 0) {
            std::cerr << "socket error: " << strerror(errno) << std::endl;
            exit(1);
        }
        int enable = 1;
        if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &enable,
                       sizeof(int)) < 0) {
            std::cerr << "setsockopt error: " << strerror(errno) << std::endl;
            exit(1);
        }
        if (bind(listen_fd, reinterpret_cast<sockaddr*>(&server_addr),
                 sizeof(server_addr)) < 0) {
            std::cerr << "bind error: " << strerror(errno) << std::endl;
            exit(1);
        }
        if (listen(listen_fd, 1024) < 0) {
            std::cerr << "listen error: " << strerror(errno) << std::endl;
            exit(1);
        }

        std::cout << "try 127.0.0.1:" << ntohs(server_addr.sin_port)
                  << std::endl;

        while (true) {
            using namespace asyncio;

            int fd = await accept(listen_fd, nullptr, nullptr, 0);
            if (fd < 0) {
                std::cerr << "accept error: " << strerror(-fd) << std::endl;
                exit(1);
            }
            std::cout << "accept fd: " << fd << std::endl;
            runtime->spawn([fd]() {
                char buf[1024];
                int n = 0;
                while ((n = await read(fd, buf, sizeof(buf)) > 0) {
                    std::cout << "fd " << fd << " recv " << n << " bytes"
                              << std::endl;
                    await write(fd, buf, n);
                }
                std::cout << "close fd: " << fd << std::endl;
                close(fd);
            });
        }
    });

    runtime->run();

    return 0;
}
