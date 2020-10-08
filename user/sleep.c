/*
 Implement the UNIX program sleep for xv6; your sleep should pause for a user-specified number of ticks. (A tick is a notion of time defined by the xv6 kernel, namely the time between two interrupts from the timer chip.) Your solution should be in the file user/sleep.c.

 1. Look at some of the other programs in user/ to see how you can obtain the command-line arguments passed to a program. If the user forgets to pass an argument, sleep should print an error message.
 2. The command-line argument is passed as a string; you can convert it to an integer using atoi (see user/ulib.c).
 3. Use the system call sleep (see user/usys.S and kernel/sysproc.c).
 4. Make sure main calls exit() in order to exit your program.
 5. Add the program to UPROGS in Makefile and compile user programs by typing make fs.img.
 Look at Kernighan and Ritchie's book The C programming language (second edition) (K&R) to learn about C.
 * */

#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char* argv[])
{
    if(argc < 2) {
        fprintf(2, "Usage: sleep ticks\n");
        exit();
    }
    int ticks=atoi(argv[1]);
    sleep(ticks);
    exit();
}

