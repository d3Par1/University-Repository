#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

/*
 * Task 8.4: What does this print?
 *
 *   int main() { int pid = fork(); printf("%d\n", pid); }
 *
 * fork() returns TWICE on success:
 *   - In the child: returns 0
 *   - In the parent: returns the child's PID
 *
 * So we see two lines of output:
 *   1) "0"            (printed by the child)
 *   2) "<child_pid>"  (printed by the parent)
 *
 * Order is undefined — depends on scheduler. Race for stdout.
 *
 * Subtle gotcha: if stdout is BUFFERED (file/pipe), the child inherits
 * the buffer and may print the parent's earlier unflushed output twice.
 * We demonstrate this by:
 *   (a) printing once BEFORE fork without flushing, then
 *   (b) printing once AFTER fork.
 */
int main(void)
{
    /* unbuffered output → no double print */
    setvbuf(stdout, NULL, _IONBF, 0);

    pid_t pid = fork();
    printf("[pid=%d]  fork returned %d\n", getpid(), pid);

    if (pid > 0) {
        /* parent: reap child to avoid zombie */
        int status;
        waitpid(pid, &status, 0);
        printf("[parent] child %d finished\n", pid);
    } else if (pid == 0) {
        /* child */
        printf("[child]  hello, my PID is %d, parent is %d\n", getpid(), getppid());
    }
    return 0;
}
