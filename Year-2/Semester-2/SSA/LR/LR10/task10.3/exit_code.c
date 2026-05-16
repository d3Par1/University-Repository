#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

/*
 * Task 10.5 (curated): child exits with code 7; parent reads it via wait().
 * Demonstrates the W*-macros and that only the LOW 8 BITS of the exit code
 * are preserved (exit(256) → wait returns 0!).
 */
int main(void)
{
    pid_t pid = fork();
    if (pid == 0) {
        printf("[child]  exiting with code 7\n");
        exit(7);
    }
    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
        printf("[parent] child %d exited normally, code=%d\n", pid, WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("[parent] child %d killed by signal %d\n", pid, WTERMSIG(status));
    }

    /* Demonstrate the 8-bit truncation */
    pid = fork();
    if (pid == 0) {
        exit(256);   /* high bits lost */
    }
    waitpid(pid, &status, 0);
    printf("[parent] child exit(256) → WEXITSTATUS=%d  (only 8 LSB preserved)\n",
           WEXITSTATUS(status));
    return 0;
}
