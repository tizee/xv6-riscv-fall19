/* Write a program that uses UNIX system calls to ``ping-pong'' a byte between
 * two processes over a pair of pipes, one for each direction. The parent sends
 * by writing a byte to parent_fd[1] and the child receives it by reading from
 * parent_fd[0]. After receiving a byte from parent, the child responds with its
 * own byte by writing to child_fd[1], which the parent then reads. Your
 * solution should be in the file user/pingpong.c. */

/*
1. Use pipe to create a pipe.
2. Use fork to create a child.
3. Use read to read from the pipe, and write to write to the pipe.
* */

#include "kernel/types.h"
#include "user/user.h"


int main(int argc, char *argv[])
{
    int pid;
    char buf[4]; // 1 byte
    int parent_fds[2];
    int child_fds[2];
    if (pipe(child_fds) != 0) {
        printf("pipe failed\n");
        exit();
    }
    if (pipe(parent_fds) != 0) {
        printf("pipe failed\n");
        exit();
    }
    if ((pid = fork()) < 0) {
        printf("fork failed\n");
        exit();
    }
    int real_pid = getpid();
    if (pid == 0) {
        // child
        if (read(parent_fds[0], &buf, 4*sizeof(char)) != 4*sizeof(char)) {
            printf("%d: child read failed \n", real_pid);
            exit();
        } else {
            printf("%d: received %s\n", real_pid,buf);
        }
        if (write(child_fds[1], "pong", 4*sizeof(char)) != 4*sizeof(char)) {
            printf("%d: child write failed \n", real_pid);
            exit();
        }
    } else if (pid > 0) {
        // parent
        if (write(parent_fds[1], "ping", 4*sizeof(char)) != 4*sizeof(char)) {
            printf("%d: parent write failed \n", real_pid);
            exit();
        }
        if (read(child_fds[0], &buf, 4*sizeof(char)) != 4*sizeof(char)) {
            printf("%d: parent read failed \n", real_pid);
            exit();
        } else {
            printf("%d: received %s\n", real_pid,buf);
        }
    }
    exit();
}
