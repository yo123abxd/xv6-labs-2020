#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "kernel/fs.h"
#include "user/user.h"

#define NULL 0

void exec_with_para(char* program, char** para) {
    /*
    fprintf(2, "program: %s\n", program);
    fprintf(2, "--------------\n");
    while (*para != NULL) {
        fprintf(2, "args: \"%s\"\n", *para);
        ++para;
    }
    fprintf(2, "--------------\n");
    return;
    */
    int pid = fork();
    if (pid == 0) {
        // child
        exec(program, para);
    } else if (pid > 0) {
        // parent
        wait(&pid);
    } else {
        fprintf(2, "fork failed");
    }
}

int main(int argc, char *argv[]) {
    if(argc < 2) {
        fprintf(2, "Usage: xargs [-n 1] <command> [args]\n");
        exit(1);
    }

    /*
    int need_split_input = 0;
    if (argc >= 4 && strcmp("-n", argv[1]) == 0 && strcmp("1", argv[2]) == 0) {
        need_split_input = 1;
    }
    */

    char buff[1024];
    int buff_len = 0;

    int LF_idx[48];
    int LF_idx_len = 0;
    while (read(0, buff + buff_len, 1) > 0) {
        /*
        if (buff[buff_len] == '\\') {
            ++buff_len;
            if (read(0, buff + buff_len, 1) > 0) {
                if (buff[buff_len] == 'n') {
                    buff[buff_len - 1] = 0;
                } else {
                    ++buff_len;
                }
            } else {
                break;
            }
            continue;
        }
        */
        if (buff[buff_len] == '\n') {
            LF_idx[LF_idx_len++] = buff_len;
            buff[buff_len] = 0;
        }
        ++buff_len;
    }
    buff[buff_len] = 0;

    char* params[MAXARG];
    int params_len = 1;
    // int argv_start_idx = need_split_input == 0 ? 2 : 4;
    int argv_start_idx = 2;
    for (; argv_start_idx < argc; ++argv_start_idx) {
        params[params_len++] = argv[argv_start_idx];
    }
    // char* program = need_split_input == 0 ? argv[1] : argv[3];
    char* program = argv[1];
    params[0] = program;
    params[params_len++] = buff;
    params[params_len] = NULL;

    exec_with_para(program, params);
    for (int i = 0; i < LF_idx_len; ++i) {
        params[params_len - 1] = buff + LF_idx[i] + 1;
        exec_with_para(program, params);
    }

    exit(0);
}
