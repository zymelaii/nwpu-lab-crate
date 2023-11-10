#pragma once

#include <winsock2.h>
#include <stdlib.h>
#include <assert.h>
#include <memory>
#include <functional>
#include <stdint.h>
#include <stddef.h>
#include <thread>
#include <vector>
#include <mutex>
#include <chrono>

using namespace std::chrono_literals;

struct TCPContext {
    static constexpr size_t BUFSIZE = 1024;

    struct Client {
        sockaddr_in addr;
        SOCKET      socket;
    };

    sockaddr_in         addr;
    SOCKET              socket;
    std::vector<Client> clients;

    int     last_sender;
    uint8_t data_buf[BUFSIZE];
    size_t  data_len;

    std::mutex data_mutex;

    void accept_once() {
        int err = listen(socket, 5);
        assert(err != SOCKET_ERROR);
        Client client{};
        int    size   = sizeof(client.addr);
        client.socket = accept(socket, (sockaddr *)&client.addr, &size);
        assert(client.socket != INVALID_SOCKET);
        clients.push_back(client);
    }

    void lock() {
        data_mutex.lock();
    }

    void unlock() {
        data_mutex.unlock();
    }

    bool recv(int client_id) {
        assert(client_id < clients.size());
        lock();
        int err =
            ::recv(clients[client_id].socket, (char *)data_buf, BUFSIZE, 0);
        if (err != SOCKET_ERROR) {
            data_len = err;
            memset(data_buf + data_len, 0, BUFSIZE - data_len);
            last_sender = client_id;
            return true;
        } else {
            unlock();
            return false;
        }
    }

    bool reply_to_last_sender() {
        assert(data_len > 0);
        int err = send(
            clients[last_sender].socket, (const char *)data_buf, data_len, 0);
        return err;
    }

    ~TCPContext() {
        assert(socket != INVALID_SOCKET);
        for (const auto &client : clients) {
            int err = closesocket(client.socket);
            assert(err == 0);
        }
        int err = closesocket(socket);
        assert(err == 0);
    }
};

struct TCPClient {
    static constexpr size_t BUFSIZE = 1024;

    SOCKET                      socket;
    std::shared_ptr<TCPContext> server;

    uint8_t data_buf[BUFSIZE];
    size_t  data_len;

    int send(uint8_t *data, size_t len) const {
        return ::send(socket, (const char *)data, len, 0);
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

    ~TCPClient() {
        assert(socket != INVALID_SOCKET);
        int err = closesocket(socket);
        assert(err == 0);
    }
};

std::shared_ptr<TCPContext> start_tcp_listener(
    const char                            *ip,
    int                                    port,
    std::function<void(TCPContext *, int)> worker,
    bool                                   start) {
    auto context = std::make_shared<TCPContext>();

    memset(&context->addr, 0, sizeof(context->addr));
    context->addr.sin_family      = AF_INET;
    context->addr.sin_port        = htons(port);
    context->addr.sin_addr.s_addr = inet_addr(ip);

    context->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (context->socket == INVALID_SOCKET) { return {}; }

    if (start) {
        int err = bind(
            context->socket, (sockaddr *)&context->addr, sizeof(context->addr));
        if (err == SOCKET_ERROR) { return {}; }

        std::thread([self = context.get()] {
            // while (true) { self->accept_once(); }
            self->accept_once();
        }).detach();

        std::thread([self = context.get(), worker] {
            while (true) {
                int n = self->clients.size();
                for (int i = 0; i < n; ++i) { worker(self, i); }
            }
        }).detach();
    }

    return context;
}

std::unique_ptr<TCPClient>
    create_tcp_client(const std::shared_ptr<TCPContext> &server) {
    auto client    = std::make_unique<TCPClient>();
    client->server = server;
    client->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client->socket == INVALID_SOCKET) { return {}; }
    int err = connect(
        client->socket,
        (sockaddr *)&client->server->addr,
        sizeof(client->server->addr));
    if (err == SOCKET_ERROR) { return {}; }
    return client;
}
