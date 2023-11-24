#include <stdint.h>
#include <assert.h>
#include <iostream>

#include "arg_parser.h"
#include "guard.h"

int main(int argc, char* argv[]) {
    WinSocketGuard guard;
    ArgParser      parser(argc, argv);

    const char* msg       = "Hello World!";
    const char* src       = nullptr;
    int         port_send = 8001;
    int         port_recv = 8002;
    int         times     = 1;

    while (!parser.empty()) {
        const char* arg = nullptr;
        if (parser.named_arg("-src", arg)) {
            src = arg;
        } else if (parser.named_arg("-port-send", arg)) {
            port_send = atoi(arg);
        } else if (parser.named_arg("-port-recv", arg)) {
            port_recv = atoi(arg);
        } else if (parser.named_arg("-m", arg)) {
            msg = arg;
        } else if (parser.named_arg("-n", arg)) {
            times = atoi(arg);
        } else {
            parser.ignore_once();
        }
    }

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    assert(sock != INVALID_SOCKET);

    sockaddr_in addr_from{};
    addr_from.sin_family      = AF_INET;
    addr_from.sin_addr.s_addr = src == nullptr ? INADDR_ANY : inet_addr(src);
    addr_from.sin_port        = htons(port_send);
    {
        auto err = bind(sock, (LPSOCKADDR)&addr_from, sizeof(addr_from));
        assert(err == 0);
    }

    {
        //! enable broadcast
        bool optval = true;
        auto err    = setsockopt(
            sock, SOL_SOCKET, SO_BROADCAST, (char*)&optval, sizeof(optval));
        assert(err == 0);
    }

    sockaddr_in addr_to{};
    addr_to.sin_family      = AF_INET;
    addr_to.sin_addr.s_addr = INADDR_BROADCAST;
    addr_to.sin_port        = htons(port_recv);

    while (times != 0) {
        auto err = sendto(
            sock,
            msg,
            strlen(msg),
            MSG_DONTROUTE,
            (sockaddr*)&addr_to,
            sizeof(addr_to));
        assert(err != SOCKET_ERROR);
        if (times > 0) { --times; }
    }

    return 0;
}
