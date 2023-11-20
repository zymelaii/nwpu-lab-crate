#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
    printf("process %d\n", getpid());
    for (int i = 0; i < 3; ++i) {
        auto pid = fork();
        if (pid < 0) { exit(-1); }
        if (pid > 0) { continue; }
        printf("process %d derives from %d\n", getpid(), getppid());
        if (i == 1) {
            for (int i = 0; i < 2; ++i) {
                auto pid = fork();
                if (pid < 0) { exit(-1); }
                if (pid > 0) { continue; }
                printf("process %d derives from %d\n", getpid(), getppid());
                exit(0);
            }
        }
        exit(0);
    }
    return 0;
}
