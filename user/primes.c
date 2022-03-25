#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {

    // maybe first fork here
    int pipe_fd[2];
    pipe(pipe_fd);
    int pid = fork();
    if (pid != 0) {
        // parent
        close(pipe_fd[0]);
        for (int i = 2; i <= 35; ++i) {
            write(pipe_fd[1], &i, sizeof(int));
        }
        close(pipe_fd[1]);
        wait(&pid);
        exit(0);
        // first parent exit
    } else {
        close(pipe_fd[1]);
        while (1) {
            // at here pipe_fd[1] (write) is closed
            // at here pipe_fd[0] (read) is opening
            // child
            const int ARR_LENGTH = 100;
            int arr[ARR_LENGTH];
            int arr_size = 0;

            int prev_fd = pipe_fd[0];
            int buff = 0;
            while (read(prev_fd, &buff, sizeof(int)) > 0) {
                arr[arr_size++] = buff;
            }
            if (arr_size == 0) {
                exit(0);
            }
            // at here pipe_fd is all closed
            close(prev_fd);
            // open new pipe_fd here
            pipe(pipe_fd);
            int next_fd = pipe_fd[1];

            // fprint before write, safe
            fprintf(2, "prime %d\n", arr[0]);

            // maybe we can fork() at here
            int sub_pid = fork();

            if (sub_pid != 0) {
                // parent
                close(pipe_fd[0]);
                int i = 1;
                for (; i < arr_size; ++i) {
                    if (arr[i] % arr[0] != 0) {
                        write(next_fd, arr + i, sizeof(int));
                    }
                }
                close(next_fd);

                int pid_child = 0;
                wait(&pid_child);
                
                exit(0);
            } else {
                // child
                close(pipe_fd[1]);
                // continue loop
            }
        }
    }
}
