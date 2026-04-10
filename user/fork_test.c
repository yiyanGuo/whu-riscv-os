#include "user.h"

void _start(void) {
    int pid = fork();

    if (pid < 0) {
        write(1, "fork failed\n", 12);
        while (1)
            ;
    }

    if (pid == 0) {
        write(1, "child\n", 6);
        exit();
    } else {
        write(1, "parent\n", 7);
        wait();
    }
    exit();
}
