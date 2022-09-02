#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

int main(int argc, char *argv[]) {
  int pid = fork();
  if (pid > 0) {
    for (int i = 0; i < 2000; i++) {
      printf("\\");
    }

  }else {
    for (int i = 0; i < 2000; i++) {
      printf("/");
    }
  }
  exit(0);
}