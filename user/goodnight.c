#include "user.h"

void _start(void) {
    int pid = fork();

    if(pid == 0) {
        write(1, "good night\n", 11);
        exit();
    } else {
        wait();
        write(1, "wan an\n", 7);
    }
    exit();
}