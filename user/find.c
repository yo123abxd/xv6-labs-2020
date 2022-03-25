#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"

#define NULL 0
#define LOG(str) fprintf(2, "%s\n", str);

char g_find_ans[1024];
int g_find_ans_len = 0;

void find(char* path, char* target_name) {
    char buf[512], *p;
    struct dirent de;
    int fd;
    struct stat st;
    if((fd = open(path, 0)) < 0){
        return;
    }
    if(fstat(fd, &st) < 0){
        close(fd);
        return;
    }
    switch(st.type){
        case T_FILE:
            close(fd);
            return;

        case T_DIR: {
            if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
                close(fd);
                return;
            }
            strcpy(buf, path);
            p = buf+strlen(buf);
            *p++ = '/';
            while (read(fd, &de, sizeof(de)) == sizeof(de)) {
                const char prev_dir[DIRSIZ] = "..";
                const char curr_dir[DIRSIZ] = ".";
                if(de.inum == 0 || strcmp(de.name, prev_dir) == 0 || strcmp(de.name, curr_dir) == 0) {
                    continue;
                }
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;
                if(stat(buf, &st) < 0) {
                    continue;
                }
                if (strcmp(de.name, target_name) == 0) {
                    memcpy(g_find_ans + g_find_ans_len, buf, strlen(buf));
                    g_find_ans_len += strlen(buf);
                    g_find_ans[g_find_ans_len++] = '\n';
                }
                if (st.type == T_DIR) {
                    find(buf, target_name);
                }
            }
            break;
        }
    }

    close(fd);
}

int main(int argc, char *argv[]) {

  if(argc < 3) {
    fprintf(2, "Usage: find <path_to_dir> <file_name>\n");
    exit(1);
  }
  
  find(argv[1], argv[2]);

  fprintf(1, g_find_ans, g_find_ans_len);

  exit(0);
}
