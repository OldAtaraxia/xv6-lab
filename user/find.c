#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// 从ls.c复制过来的格式化文件名的东西
char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

// 主题逻辑
/*
struct dirent {
  ushort inum;
  char name[DIRSIZ];
};
*/
void find(char *path, char *filename) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(path, 0)) < 0){
        fprintf(2, "ls: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0){
        fprintf(2, "ls: cannot stat %s\n", path);
        close(fd);
        return;
    }
    // 根据path的属性选择进行递归调用
    switch(st.type){
      case T_FILE:
        // 判断当前文件的结尾与`filename`是否匹配
        if (strcmp(path + strlen(path) - strlen(filename), filename) == 0) {
            printf("%s\n", path);
        }
        break;
    
      case T_DIR:
        // 递归为目录下的每个成员调用find命令
        if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
          printf("ls: path too long\n");
          break;
        }

        strcpy(buf, path);
        p = buf+strlen(buf);
        *p++ = '/';

        // 从目录文件读取内容
        while(read(fd, &de, sizeof(de)) == sizeof(de)){
          if(de.inum == 0)
            continue;
          memmove(p, de.name, DIRSIZ);
          p[DIRSIZ] = 0;
          if(stat(buf, &st) < 0){
            printf("ls: cannot stat %s\n", buf);
            continue;
          }
          //   不要进入`.`和`..`
        //   printf("%s %s\n", fmtname(buf), buf);
        //   printf("%d %d\n", strcmp(buf + strlen(buf) - 2, "/."), strcmp(buf + strlen(buf) - 3, "/.."));
          if (strcmp(buf + strlen(buf) - 2, "/.") == 0 || strcmp(buf + strlen(buf) - 3, "/..") == 0) {
            // printf("now find . / ..\n");
            continue;
          }
          find(buf, filename);
        }
        break;
      }
      close(fd);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("find need three argument\n");
        exit(0);
    }
    find(argv[1], argv[2]);
    exit(0);
}
