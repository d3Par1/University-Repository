#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

/*
 * Task 10.1: After fork(), parent modifies a global variable.
 * Both processes print it. We see that the address can be IDENTICAL
 * yet the values DIFFER — because COW (copy-on-write) made the page
 * private the moment one side wrote to it.
 */
int counter = 1000;

int main(void)
{
    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }

    if (pid > 0) {
        /* parent — bump the counter */
        sleep(1);             /* let child read first */
        counter = 9999;
        printf("[parent] pid=%d counter=%d &counter=%p\n",
               getpid(), counter, (void *)&counter);
        wait(NULL);
    } else {
        printf("[child]  pid=%d counter=%d &counter=%p (before parent updates)\n",
               getpid(), counter, (void *)&counter);
        sleep(2);
        printf("[child]  pid=%d counter=%d &counter=%p (after parent's update)\n",
               getpid(), counter, (void *)&counter);
    }
    return 0;
}
