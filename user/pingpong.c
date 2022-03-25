#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int pipe_fd[2];
    int sync_pipe_fd[2];
    pipe(pipe_fd);
    pipe(sync_pipe_fd);
    int pid;
    if ((pid = fork()) != 0) { // parent
        write(pipe_fd[1], "1", 1);

        char buff[10];

        read(sync_pipe_fd[0], buff, 1);
        read(pipe_fd[0], buff, 1);

        fprintf(2, "%d: received pong\n", getpid());

        wait(&pid);

        // fprintf(2, "parent exit\n");

        exit(0);
    } else { // child
        char buff[10];

        read(pipe_fd[0], buff, 10);

        fprintf(2, "%d: received ping\n", getpid());

        write(sync_pipe_fd[1], buff, 1);
        write(pipe_fd[1], buff, 1);

        // fprintf(2, "child exit\n");
        
        exit(0);
    }
}
