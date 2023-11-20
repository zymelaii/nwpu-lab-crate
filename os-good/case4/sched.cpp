#include <iostream>
#include <limits>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

void loop_write(char ch) {
    while (true) {
        putchar(ch);
        fflush(stdout);
        for (int i = 0; i < std::numeric_limits<int>::max() / 4; ++i) {}
    }
}

int main(int argc, char *argv[]) {
    bool resort_priority = false;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-p") == 0) { resort_priority = true; }
    }

    resort_priority && (nice(9)); //<! root process
    pid_t p = fork();
    if (p < 0) {
        perror("fork fail");
        exit(-1);
    } else if (p == 0) {
        resort_priority && (nice(3)); //<! l1 child process
        pid_t pp = fork();
        if (pp < 0) {
            perror("fork fail");
            exit(-1);
        } else if (pp == 0) {
            resort_priority && (nice(6)); //<! l2 child process
            loop_write('c');
        } else {
            loop_write('b');
        }
    } else {
        loop_write('a');
    }
    return 0;
}
