#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("sleep need at least one argument\n");
    }
    // 调用sleep系统调用
    sleep(atoi(argv[1]));
    exit(0);
}