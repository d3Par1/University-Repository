#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

/*
 * Task 10.18 (curated): SIGCHLD handler that asynchronously reports
 * child terminations.
 *
 * Critical correctness:
 *   1) Handler MUST loop on waitpid(-1, ..., WNOHANG) — multiple SIGCHLDs
 *      can coalesce into one if they arrive close together (signals are
 *      a 1-bit "pending" flag, not a queue, for standard signals).
 *   2) Only async-signal-safe functions inside the handler (write, not
 *      printf). Here we cheat slightly for clarity, but real code uses
 *      write(STDERR, ...) and snprintf-to-buffer.
 *   3) sigaction with SA_NOCLDSTOP avoids notifications for stopped
 *      (not-terminated) children.
 */
static volatile sig_atomic_t reaped = 0;

static void on_sigchld(int sig)
{
    (void)sig;
    int saved_errno = errno;
    pid_t pid;
    int status;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        char buf[128];
        int n = snprintf(buf, sizeof buf,
                         "[handler] reaped pid=%d exit=%d\n",
                         pid, WIFEXITED(status) ? WEXITSTATUS(status) : -1);
        (void)!write(STDOUT_FILENO, buf, n);
        ++reaped;
    }
    errno = saved_errno;
}

int main(void)
{
    struct sigaction sa = {0};
    sa.sa_handler = on_sigchld;
    sa.sa_flags = SA_NOCLDSTOP | SA_RESTART;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGCHLD, &sa, NULL) < 0) { perror("sigaction"); return 1; }

    int N = 4;
    for (int i = 0; i < N; ++i) {
        pid_t pid = fork();
        if (pid == 0) {
            sleep(1 + i);          /* staggered durations */
            _exit(10 + i);
        }
    }
    printf("[parent] spawned %d children, waiting via SIGCHLD handler\n", N);

    while (reaped < N) pause();    /* wait for signals */
    printf("[parent] all %d children reaped, exiting\n", N);
    return 0;
}
