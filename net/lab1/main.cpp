#include <stdlib.h>
#include <filesystem>

#include "impl.h"
#include "guard.h"

namespace fs = std::filesystem;

int main(int argc, char *argv[]) {
    WinSocketGuard guard;

    bool        use_udp             = true;
    bool        video_trans         = false;
    int         port                = 5055;
    const char *video_file          = nullptr;
    bool        enable_random_pause = false;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-udp") == 0) {
            use_udp = true;
        } else if (strcmp(argv[i], "-tcp") == 0) {
            use_udp = false;
        } else if (strcmp(argv[i], "-p") == 0) {
            ++i;
            if (i >= argc) { break; }
            port = atoi(argv[i]);
        } else if (strcmp(argv[i], "-video") == 0) {
            video_trans = true;
        } else if (strcmp(argv[i], "-i") == 0) {
            ++i;
            if (i >= argc) { break; }
            video_file = argv[i];
        } else if (strcmp(argv[i], "-pause") == 0) {
            enable_random_pause = true;
        }
    }

    if (video_trans && (video_file == nullptr || !fs::exists(video_file))) {
        fprintf(stderr, "ERROR: no video file to transfer\n");
        exit(-1);
    }

    if (use_udp) {
        auto udp_server =
            start_udp_listener("127.0.0.1", port, udp_server_worker);
        assert(udp_server.get() != nullptr);
        if (video_trans) {
            std::thread(
                udp_file_trans_routine,
                udp_server,
                video_file,
                enable_random_pause)
                .detach();
        } else {
            std::thread(udp_client_routine, udp_server).detach();
        }
    } else {
        auto tcp_server =
            start_tcp_listener("127.0.0.1", port, tcp_server_worker);
        assert(tcp_server.get() != nullptr);
        if (video_trans) {
            std::thread(
                tcp_file_trans_routine,
                tcp_server,
                video_file,
                enable_random_pause)
                .detach();
        } else {
            std::thread(tcp_client_routine, tcp_server).detach();
        }
    }

    while (true) {}

    return 0;
}
