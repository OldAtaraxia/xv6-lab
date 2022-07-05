#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#define MSGSIZE 16

int main() {
    int p[2];
    char bytes[MSGSIZE]; 
    pipe(p);

    int pid = fork();
    if (pid == 0) {
        // 子进程
        read(p[0], bytes, 5);
        printf("%d: received ping\n", getpid());
        write(p[1], "pong", 5);
        close(p[0]);
        close(p[1]);
        exit(0);
    } else {
        // 父进程
        write(p[1], "ping", 5);
        close(p[1]);
        wait(0);
        read(p[0], bytes, 5);
        printf("%d: received pong\n", getpid());
        close(p[0]);
        exit(0);
    }
}