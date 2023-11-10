#pragma once

#include <winsock2.h>
#include <stdlib.h>
#include <assert.h>
#include <memory>
#include <functional>
#include <stdint.h>
#include <stddef.h>
#include <thread>

struct UDPContext {
    static constexpr size_t BUFSIZE = 1024;

    sockaddr_in addr;
    SOCKET      socket;

    sockaddr_in last_sender;
    uint8_t     data_buf[BUFSIZE];
    size_t      data_len;

    bool recv() {
        int size = sizeof(last_sender);
        int err  = recvfrom(
            socket,
            (char *)data_buf,
            BUFSIZE,
            0,
            (sockaddr *)&last_sender,
            &size);
        if (err != SOCKET_ERROR) {
            data_len = err;
            memset(data_buf + data_len, 0, BUFSIZE - data_len);
        }
        return err != SOCKET_ERROR;
    }

    bool reply_to_last_sender() {
        assert(data_len > 0);
        return sendto(
            socket,
            (const char *)data_buf,
            data_len,
            0,
            (sockaddr *)&last_sender,
            sizeof(last_sender));
    }

    ~UDPContext() {
        assert(socket != INVALID_SOCKET);
        int err = closesocket(socket);
        assert(err == 0);
    }
};

struct UDPClient {
    static constexpr size_t BUFSIZE = 1024;

    SOCKET                      socket;
    std::shared_ptr<UDPContext> server;

    uint8_t data_buf[BUFSIZE];
    size_t  data_len;

    int send(uint8_t *data, size_t len) const {
        return sendto(
            socket,
            (const char *)data,
            len,
            0,
            (sockaddr *)&server->addr,
            sizeof(server->addr));
    }

    bool recv_from_server() {
        int size = sizeof(server->addr);
        int err  = recvfrom(
            socket,
            (char *)data_buf,
            BUFSIZE,
            0,
            (sockaddr *)&server->addr,
            &size);
        if (err != SOCKET_ERROR) {
            data_len = err;
            memset(data_buf + data_len, 0, BUFSIZE - data_len);
        }
        return err != SOCKET_ERROR;
    }

    ~UDPClient() {
        assert(socket != INVALID_SOCKET);
        int err = closesocket(socket);
        assert(err == 0);
    }
};

std::shared_ptr<UDPContext> start_udp_listener(
    const char *ip, int port, std::function<void(UDPContext *)> worker) {
    auto context = std::make_shared<UDPContext>();

    memset(&context->addr, 0, sizeof(context->addr));
    context->addr.sin_family      = AF_INET;
    context->addr.sin_port        = htons(port);
    context->addr.sin_addr.s_addr = inet_addr(ip);

    context->socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (context->socket == INVALID_SOCKET) { return {}; }

    int err = bind(
        context->socket, (sockaddr *)&context->addr, sizeof(context->addr));
    if (err == SOCKET_ERROR) { return {}; }

    std::thread(worker, context.get()).detach();

    return context;
}

std::unique_ptr<UDPClient>
    create_udp_client(const std::shared_ptr<UDPContext> &server) {
    auto client    = std::make_unique<UDPClient>();
    client->server = server;
    client->socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client->socket == INVALID_SOCKET) { return {}; }
    return client;
}
