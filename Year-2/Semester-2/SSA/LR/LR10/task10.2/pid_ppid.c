#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

/*
 * Task 10.2: Child prints its PID and PPID, then exits.
 * Parent reaps via wait() so we don't leave a zombie.
 */
int main(void)
{
    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }

    if (pid == 0) {
        printf("[child]  my pid=%d, parent pid=%d\n", getpid(), getppid());
        _exit(0);
    }
    printf("[parent] my pid=%d, just forked child=%d\n", getpid(), pid);
    wait(NULL);
    return 0;
}
