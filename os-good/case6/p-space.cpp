#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int g_shared = 0;

void run_task1(int &shared) {
    while (true) {
        shared = 1 - shared;
        printf("%d: shared=%d\n", getpid(), shared);
    }
}

void run_task2(int &shared) {
    auto pid = fork();
    if (pid < 0) { exit(-1); }
    while (true) {
        if (pid == 0) { //<! child
            shared = 1 - shared;
        } else if (pid > 0) { //<! parent
            shared = shared + 1;
        }
        printf("%d: shared=%d\n", getpid(), shared);
    }
}

void dispatch(int step, int &shared) {
    switch (step) {
        case 1: {
            run_task1(shared);
        } break;
        case 2:
        case 3: {
            run_task2(shared);
        } break;
        default: {
            exit(-1);
        } break;
    }
}

int main(int argc, char *argv[]) {
    int step = 1;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-i") == 0) {
            if (++i >= argc) { exit(-1); }
            step = atoi(argv[i]);
        }
    }
    if (!(step >= 1 && step <= 3)) { exit(-1); }

    int shared = 0;
    dispatch(step, step == 3 ? g_shared : shared);

    return 0;
}
