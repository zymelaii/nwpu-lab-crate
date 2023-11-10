#pragma once

#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <thread>
#include <fstream>
#include <limits.h>
#include <random>

#include "udp.h"
#include "tcp.h"

using namespace std::chrono_literals;

void udp_server_worker(UDPContext *self) {
    while (true) {
        if (!self->recv()) { continue; }
        if (!isprint(self->data_buf[0])) { continue; }

        printf(
            "[SERVER] from: %s:%d content: %s\n",
            inet_ntoa(self->last_sender.sin_addr),
            ntohs(self->last_sender.sin_port),
            (char *)self->data_buf);

        const char *req = nullptr;

        if ((req = strstr((char *)self->data_buf, "/echo "))) {
            strcpy((char *)self->data_buf, req + strlen("/echo "));
            self->data_len = strlen((char *)self->data_buf);
            self->reply_to_last_sender();
        } else if ((req = strstr((char *)self->data_buf, "/trans "))) {
            const char *ack = "/trans-done";
            char        file[PATH_MAX + 5]{"recv_"};
            strcat_s(file, req + strlen("/trans "));
            std::ofstream ofs(file, std::ios_base::binary);
            size_t        total = 0;
            while (true) {
                bool ok = self->recv();
                assert(ok);
                if (memcmp(self->data_buf, ack, strlen(ack)) == 0) { break; }
                total += self->data_len;
                printf(
                    "[Server] receive %zu bytes, %zu in total\n",
                    self->data_len,
                    total);
                ofs.write((char *)self->data_buf, self->data_len);
            }
            puts("[Server] file transfer done");
        }
    }
}

void udp_client_routine(const std::shared_ptr<UDPContext> &server) {
    auto client = create_udp_client(server);
    assert(client.get() != nullptr);

    std::thread([client = client.get()]() {
        while (true) {
            if (!client->recv_from_server()) { continue; }
            printf(
                "[CLIENT] from: %s:%d content: %s\n",
                inet_ntoa(client->server->addr.sin_addr),
                ntohs(client->server->addr.sin_port),
                (char *)client->data_buf);
        }
    }).detach();

    char buf[1024]{};
    while (true) {
        std::cin.getline(buf, sizeof(buf));
        int err = client->send((uint8_t *)buf, strlen(buf));
        assert(err != SOCKET_ERROR);
    }
}

void udp_file_trans_routine(
    const std::shared_ptr<UDPContext> &server,
    const char                        *file,
    bool                               enable_random_pause) {
    std::ifstream ifs(file, std::ios::binary);
    assert(ifs.is_open());

    auto client = create_udp_client(server);
    assert(client.get() != nullptr);

    uint8_t buf[1024]{};
    sprintf((char *)buf, "/trans %s", file);
    client->send(buf, strlen((char *)buf));

    bool         pause_done = false;
    std::mt19937 random(std::random_device{}());

    while (!ifs.eof()) {
        std::this_thread::sleep_for(10ms);
        ifs.read((char *)buf, sizeof(buf));
        int err = client->send(buf, sizeof(buf));
        assert(err != SOCKET_ERROR);
        if (enable_random_pause && !pause_done && (random() % 5 == 0)) {
            auto dur = std::chrono::seconds(random() % 3 + 3);
            printf("[Client] sleep for %lld seconds\n", dur.count());
            std::this_thread::sleep_for(dur);
            pause_done = true;
        }
    }

    std::this_thread::sleep_for(10ms);
    strcpy((char *)buf, "/trans-done");
    client->send(buf, strlen((char *)buf));
}

void tcp_file_trans_routine(
    const std::shared_ptr<TCPContext> &server,
    const char                        *file,
    bool                               enable_random_pause) {
    std::ifstream ifs(file, std::ios::binary);
    assert(ifs.is_open());

    auto client = create_tcp_client(server);
    assert(client.get() != nullptr);

    uint8_t buf[1024]{};
    sprintf((char *)buf, "/trans %s", file);
    client->send(buf, strlen((char *)buf));

    bool         pause_done = false;
    std::mt19937 random(std::random_device{}());

    while (!ifs.eof()) {
        std::this_thread::sleep_for(10ms);
        ifs.read((char *)buf, sizeof(buf));
        int err = client->send(buf, sizeof(buf));
        assert(err != SOCKET_ERROR);
        if (enable_random_pause && !pause_done && (random() % 5 == 0)) {
            auto dur = std::chrono::seconds(random() % 3 + 3);
            printf("[Client] sleep for %lld seconds\n", dur.count());
            std::this_thread::sleep_for(dur);
            pause_done = true;
        }
    }

    std::this_thread::sleep_for(10ms);
    strcpy((char *)buf, "/trans-done");
    client->send(buf, strlen((char *)buf));
}

void tcp_server_worker(TCPContext *self, int client_id) {
    if (!self->recv(client_id)) { return; }
    if (!isprint(self->data_buf[0])) { return; }

    assert(self->last_sender == client_id);
    auto [sender, socket] = self->clients[client_id];

    printf(
        "[SERVER] from(%d): %s:%d content: %s\n",
        client_id,
        inet_ntoa(sender.sin_addr),
        ntohs(sender.sin_port),
        (char *)self->data_buf);

    const char *req = nullptr;

    if ((req = strstr((char *)self->data_buf, "/echo "))) {
        strcpy((char *)self->data_buf, req + strlen("/echo "));
        self->data_len = strlen((char *)self->data_buf);
        self->reply_to_last_sender();
    } else if ((req = strstr((char *)self->data_buf, "/trans "))) {
        const char *ack = "/trans-done";
        char        file[PATH_MAX + 5]{"recv_"};
        strcat_s(file, req + strlen("/trans "));
        std::ofstream ofs(file, std::ios_base::binary);
        size_t        total = 0;
        while (true) {
            self->unlock();
            bool ok = self->recv(client_id);
            if (memcmp(self->data_buf, ack, strlen(ack)) == 0) { break; }
            assert(ok);
            total += self->data_len;
            printf(
                "[Server] receive %zu bytes, %zu in total\n",
                self->data_len,
                total);
            ofs.write((char *)self->data_buf, self->data_len);
        }
        puts("[Server] file transfer done");
    }

    self->unlock();
}

void tcp_client_routine(const std::shared_ptr<TCPContext> &server) {
    auto client = create_tcp_client(server);
    assert(client.get() != nullptr);

    std::thread([client = client.get()]() {
        while (true) {
            if (!client->recv_from_server()) { continue; }
            printf(
                "[CLIENT] from: %s:%d content: %s\n",
                inet_ntoa(client->server->addr.sin_addr),
                ntohs(client->server->addr.sin_port),
                (char *)client->data_buf);
        }
    }).detach();

    char buf[1024]{};
    while (true) {
        std::cin.getline(buf, sizeof(buf));
        int err = client->send((uint8_t *)buf, strlen(buf));
        assert(err != SOCKET_ERROR);
    }
}
