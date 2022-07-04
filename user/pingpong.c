#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main() {
    int p[2];
    char* ping = "ping";
    char* pong = "pong";
    char* bytes = malloc(6 * sizeof(char)); 
    pipe(p);

    int pid = fork();
    if (pid == 0) {
        // 子进程
        read(p[0], bytes, 5);
        printf("%d: reveived %s\n", getpid(), bytes);
        write(p[1], pong, 5);
        close(p[0]);
        close(p[1]);
        exit(0);
    } else {
        // 父进程
        write(p[1], ping, 5);
        read(p[0], bytes, 5);
        printf("%d: reveived %s\n", getpid(), bytes);
        close(p[0]);
        close(p[1]);
        exit(0);
    }
}