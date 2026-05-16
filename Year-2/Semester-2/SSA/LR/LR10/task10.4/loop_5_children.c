#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

/*
 * Task 10.6 (curated): Create 5 children in a loop. Each prints its number
 * and exits. Parent waits for them all.
 *
 * Critical correctness point: the `break` after a successful child fork
 * prevents the classic "fork bomb" — without it, child also enters the
 * loop and creates ANOTHER 5 children, multiplying exponentially.
 */
int main(void)
{
    int N = 5;
    for (int i = 0; i < N; ++i) {
        pid_t pid = fork();
        if (pid < 0) { perror("fork"); break; }
        if (pid == 0) {
            printf("[child %d/%d] pid=%d, parent=%d\n", i + 1, N, getpid(), getppid());
            _exit(i);            /* exit code = child index for traceability */
        }
    }
    /* parent reaps all */
    int status;
    pid_t pid;
    while ((pid = wait(&status)) > 0) {
        if (WIFEXITED(status))
            printf("[parent] child %d exited code=%d\n", pid, WEXITSTATUS(status));
    }
    return 0;
}
