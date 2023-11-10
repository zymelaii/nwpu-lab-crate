#pragma once

#include <winsock2.h>
#include <stdlib.h>

class WinSocketGuard {
public:
    WinSocketGuard() {
        WSADATA wsadata{};

        int err = WSAStartup(MAKEWORD(2, 2), &wsadata);
        if (err != 0) { exit(err); }
    }

    ~WinSocketGuard() {
        WSACleanup();
    }
};
