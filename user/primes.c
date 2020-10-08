/* Write a concurrent version of prime sieve using pipes. This idea is due to Doug McIlroy, inventor of Unix pipes. The picture halfway down this page and the surrounding text explain how to do it. Your solution should be in the file user/primes.c.
 *
 * Be careful to close file descriptors that a process doesn't need, because otherwise your program will run xv6 out of resources before the first process reaches 35.
Once the first process reaches 35, you should arrange that the pipeline terminates cleanly, including all children (Hint: read will return an end-of-file when the write-side of the pipe is closed).
It's simplest to directly write 32-bit ints to the pipes, rather than using formatted ASCII I/O.
You should create the processes in the pipeline as they are needed.
 * */

#include "kernel/types.h"
#include "user/user.h"

void generator()
{
    int i = 2;
    while (i<36) {
        write(1, &i, sizeof(int));
        i++;
    }
}

void cull(int p)
{
    int buf;
    while (read(0, &buf, sizeof(int))) {
        if(buf%p!=0) {
            write(1,&buf, sizeof(int));
        }
    }
}

// k=0 stdin k=1 stdout
// fd[2]
void redirectTo(int k, int* fd)
{
    close(k); // release k and make it available
    dup(fd[k]);
    close(fd[0]); // release fd stdout and make it available
    close(fd[1]); // release fd stdin and make it available
}

void sieve()
{
    int buf;
    if (read(0, &buf, sizeof(int))==0) {
        exit();
    }
    int pid;
    int fds[2];
    printf("primes %d\n",buf);
    // create new pipe for new prime
    if (pipe(fds)!=0) {
        fprintf(2, "pipe() failed\n");
        exit();
    }
    // fork new child process for processing new prime each time
    if((pid= fork())<0) {
        fprintf(2, "fork() failed\n");
        exit();
    } else if (pid==0) {
        redirectTo(0, fds);
        // use fds[0] to replace stdin
        sieve();
    } else {
        redirectTo(1, fds);
        // use fds[1] to replace stdout
        cull(buf);
    }
    exit();
}

int main(int argc, char* argv[])
{
    int pid;
    int fds[2];
    if (pipe(fds)!=0) {
        fprintf(2, "pipe() failed\n");
        exit();
    }
    if((pid= fork())<0) {
        fprintf(2, "fork() failed\n");
        exit();
    } else  if (pid==0) {
        redirectTo(1, fds);
        // use fds[1] to replace stdout
        generator();
    } else {
        redirectTo(0, fds);
        // use fds[0] to replace stdin
        sieve();
    }
    exit();
}

