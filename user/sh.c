#include "user.h"

void _start(void){
    int n;
    int i;
    int pid;
    char program_name[16];

    while(1) {
        write(1, "$: ", 3);
        n = read(0, program_name, sizeof(program_name));
        if(n <= 0)
            continue;

        for(i = 0; i < n; i++) {
            if(program_name[i] == '\n' || program_name[i] == '\r') {
                program_name[i] = '\0';
                break;
            }
        }
        if(i == n) {
            if(n == sizeof(program_name))
                program_name[n - 1] = '\0';
            else
                program_name[n] = '\0';
        }

        if(program_name[0] == '\0')
            continue;

        pid = fork();
        if(pid < 0) {
            write(1, "fork failed\n", 12);
            continue;
        }
        if(pid == 0) {
            if(exec(program_name) < 0)
                write(1, "exec failed\n", 12);
            exit();
        } else {
            wait();
        }
    }
}
