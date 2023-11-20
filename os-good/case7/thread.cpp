#include <iostream>
#include <string.h>
#include <stddef.h>
#include <thread>
#include <limits>

int g_shared = 0;

size_t thread_hash_id() {
    auto id = std::this_thread::get_id();
    return std::hash<decltype(id)>{}(id);
}

void delay() {
    for (int i = 0; i < std::numeric_limits<int>::max() / 4; ++i) {}
}

void job(int &shared) {
    int local_var = 0;
    int sign      = (thread_hash_id() & 1) ? 1 : -1;
    while (true) {
        --shared;
        local_var += sign * (thread_hash_id() & 0xf);
        printf(
            "%zu: shared=%d local_var=%d\n",
            thread_hash_id(),
            shared,
            local_var);
        delay();
    }
}

void run_task(int &shared) {
    while (true) {
        ++shared;
        printf("%zu: shared=%d\n", thread_hash_id(), shared);
        delay();
    }
}

void dispatch(int step, int &shared) {
    std::thread(job, std::ref(shared)).detach();
    std::thread(job, std::ref(shared)).detach();
    run_task(shared);
}

int main(int argc, char *argv[]) {
    int step = 1;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-i") == 0) {
            if (++i >= argc) { exit(-1); }
            step = atoi(argv[i]);
        }
    }
    if (!(step >= 1 && step <= 4)) { exit(-1); }

    int shared = 0;
    dispatch(step, step == 3 ? shared : g_shared);

    return 0;
}
