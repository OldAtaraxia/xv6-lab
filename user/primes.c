#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define N 35

void stage(int pleft) {
    // printf("now n == %d\n", n);
    int left = -1, n = -1;
    if (read(pleft, &n, sizeof(int)) == 0) {
        exit(0);
    };
    printf("prime %d\n", n);

    
    int p[2];
    pipe(p);
    while (read(pleft, &left, sizeof(int)) != 0) {
        if (left % n != 0) {
            write(p[1], &left, sizeof(int));
            // printf("write %d to the next stage\n", left);
        }
    }
    close(pleft);
    close(p[1]);
    // printf("will create the next process\n");
    int pid = fork();
    // printf("pid == %d", pid);
    if (pid == 0) {
        // printf("will come to the next stage\n");
        stage(p[0]);
    } else {
        close(p[0]);
        wait(0);
    }
    exit(0);
}

int main() {
    int p[2];
    pipe(p);
    for (int i = 2; i <= N; i++) {
        write(p[1], &i, sizeof(int));
    }
    close(p[1]);
    int pid = fork();
    if (pid == 0) {
        stage(p[0]);
    } else {
        close(p[1]);
        close(p[0]);
        wait(0);
    }
    exit(0);
}