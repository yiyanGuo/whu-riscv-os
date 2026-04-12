#include "user.h"

#define MAXARG 16

static int parsecmd(char *cmd, char *argv[], int maxarg);

int _start(void) {
    static char buf[100];
    char *argv[MAXARG];
    int n, argc, pid;

    // TODO: open console

    while(1) {
        write(1, "$ ", 2);
        n = read(0, buf, sizeof(buf));
        if(n <= 0)
            continue;
        if(n >= sizeof(buf))
            n = sizeof(buf) - 1;
        buf[n] = '\0';

        char *cmd = buf;

        // 处理空格
        while(*cmd == ' ' || *cmd == '\t')
            cmd++;
        if(*cmd == '\n')
            continue;

        // 解析cmd
        argc = parsecmd(cmd, argv, MAXARG);
        if(argc <= 0) {
            continue;
        }

        // 执行cmd
        pid = fork();
        if(pid < 0) {
            write(1, "fork failed\n", 12);
            continue;
        }
        if(pid == 0) {
            exec(argv[0], argv);
            write(1, "exec failed\n", 12);
            exit();
        }
        wait();
    }
}

static int parsecmd(char *cmd, char *argv[], int maxarg) {
    int argc = 0;
    while(*cmd) {
        while(*cmd == ' ' || *cmd == '\t' || *cmd == '\n' || *cmd == '\r') {
            cmd++;
        }
        if(*cmd == '\0')
            break;
        if(argc >= maxarg - 1)
            break;

        argv[argc] = cmd;
        argc++;

        while(*cmd != '\0' &&
              *cmd != ' ' &&
              *cmd != '\t' &&
              *cmd != '\n' &&
              *cmd != '\r')
            cmd++;
        if(*cmd != '\0') {
            *cmd = '\0';
            cmd++;
        }
    }
    argv[argc] = 0;
    return argc;
}

// void _start(){
//     int n;
//     int i;
//     int pid;
//     char program_name[16];

//     while(1) {
//         write(1, "$: ", 3);
//         n = read(0, program_name, sizeof(program_name));
//         if(n <= 0)
//             continue;

//         for(i = 0; i < n; i++) {
//             if(program_name[i] == '\n' || program_name[i] == '\r') {
//                 program_name[i] = '\0';
//                 break;
//             }
//         }
//         if(i == n) {
//             if(n == sizeof(program_name))
//                 program_name[n - 1] = '\0';
//             else
//                 program_name[n] = '\0';
//         }

//         if(program_name[0] == '\0')
//             continue;

//         pid = fork();
//         if(pid < 0) {
//             write(1, "fork failed\n", 12);
//             continue;
//         }
//         if(pid == 0) {
//             if(exec(program_name) < 0)
//                 write(1, "exec failed\n", 12);
//             exit();
//         } else {
//             wait();
//         }
//     }
// }
