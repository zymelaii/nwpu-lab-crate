/*!
 * \brief port scanner cli
 * \author 周上行Ryer
 * \date 2023-11-17
 */

#include <winsock2.h>
#include <iostream>
#include <thread>
#include <algorithm>
#include <regex>
#include <atomic>
#include <chrono>

#include "guard.h"
#include "multi_range_queue.h"
#include "arg_parser.h"

using namespace std::chrono_literals;

void parse_ranges(MultiRangeQueue &queue, const char *ranges) {
    std::cmatch results{};
    std::regex  re(R"((\d+)(?:-(\d+))?)");
    while (std::regex_search(ranges, results, re)) {
        auto from = atoi(results[1].str().c_str());
        if (results[2].length() != 0) {
            auto to = atoi(results[2].str().c_str());
            queue.push(from, to);
        } else {
            queue.push(from, from);
        }
        ranges += results[0].length();
        if (ranges[0] == ',') {
            ++ranges;
        } else {
            break;
        }
    }
}

/*!
 * \retval 0 success
 * \retval 1 failed
 * \retval -1 unknown error
 */
int telport(const char *addr, int port) {
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock == INVALID_SOCKET) { return -1; }

    sockaddr_in server_addr{};
    server_addr.sin_family           = AF_INET;
    server_addr.sin_port             = htons(port);
    server_addr.sin_addr.S_un.S_addr = inet_addr(addr);

    connect(sock, (sockaddr *)&server_addr, sizeof(server_addr));

    fd_set write{};
    FD_ZERO(&write);
    FD_SET(sock, &write);

    timeval timeout{.tv_sec = 1, .tv_usec = 0};

    int result = select(0, NULL, &write, NULL, &timeout);

    closesocket(sock);

    return result > 0 ? 0 : 1;
}

void telport_job(
    std::atomic_int &alive, MultiRangeQueue &ports, const char *addr) {
    while (!ports.empty()) {
        int         port = ports.pop();
        int         resp = telport(addr, port);
        const char *msg  = nullptr;
        switch (resp) {
            case 0: {
                msg = "connected";
            } break;
            case 1: {
                msg = "timeout";
            } break;
            case -1:
            default: {
                msg = "unknown error";
            } break;
        }
        printf("telport %d: %s\n", port, msg);
    }
    --alive;
}

int main(int argc, char *argv[]) {
    WinSocketGuard guard;

    ArgParser parser(argc, argv);

    MultiRangeQueue ports;
    int             total_threads = 1;
    const char     *addr          = "127.0.0.1";

    while (!parser.empty()) {
        const char *arg = nullptr;
        if (parser.named_arg("-ranges", arg)) {
            parse_ranges(ports, arg);
        } else if (parser.named_arg("-job", arg)) {
            total_threads = std::clamp(atoi(arg), 1, 32);
        } else if (parser.named_arg("-ip", arg)) {
            addr = arg;
        } else {
            parser.ignore_once();
        }
    }

    std::atomic_int alive{0};
    for (int i = 0; i < total_threads; ++i) {
        ++alive;
        std::thread(telport_job, std::ref(alive), std::ref(ports), addr)
            .detach();
    }

    while (alive > 0) { std::this_thread::sleep_for(500ms); }

    return 0;
}
