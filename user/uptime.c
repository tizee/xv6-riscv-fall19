/* Optional: write an uptime program that prints the uptime in terms of ticks using the uptime system call. */

#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char* argv[])
{
    int time=uptime();
    fprintf(1, "%d \n", time);
    exit();
}

