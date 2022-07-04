#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char* argv[]) {
    char buf[512]; // 相当于读入时的缓冲池
    char* newargv[MAXARG];
    char c;
    int idx = 0, args = argc - 1; // 分别用于buf和args的计数
    if(argc < 2) {
        printf("wrong argvs format\n");
    }
    for(int i = 1; i < argc; i++) {
        newargv[i - 1] = argv[i];
    }
    while(read(0, &c, sizeof(char)) != 0 ) {
        if(c == ' ' || c == '\n') {
            buf[idx++] = '\0';
            newargv[args] = malloc(sizeof(char) * strlen(buf));
            strcpy(newargv[args++], buf);
            idx = 0;
            if(c == '\n') {
                newargv[args++] = 0;
                if(fork() == 0) {
                    // printf("%s\n", newargv[0]);
                    exec(newargv[0], newargv);
                    exit(0);
                }
                wait((int *)0);           
                args = argc - 1; //初始值了2333
            }
        } else {
            buf[idx++] = c;
        }
    } 
    if(args != argc - 1) {
        // 最后一行不为空
        newargv[args++] = 0;
        if(fork() == 0) {
            exec(newargv[0], newargv);
            exit(0);
        }
        wait((int *)0);           
    }
    exit(0);
}