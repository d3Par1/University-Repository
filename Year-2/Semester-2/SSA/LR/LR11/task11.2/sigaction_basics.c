#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

/*
 * Task 11.2: Demonstrate why sigaction() is preferred over signal().
 *
 * Problems with the old signal():
 *   1) Some implementations (System V) RESET the handler to SIG_DFL after
 *      one delivery → handler runs once, then ignored / kills program.
 *   2) No control over which signals are blocked during the handler →
 *      reentrance bugs.
 *   3) No way to get siginfo_t (no fault address, no sender PID).
 *   4) Auto-restart of interrupted syscalls is unspecified.
 *
 * sigaction() fixes all of this with sa_flags, sa_mask, sa_sigaction.
 */
static volatile sig_atomic_t got = 0;

static void on_usr1(int sig, siginfo_t *info, void *ctx)
{
    (void)sig; (void)ctx;
    char buf[128];
    int n = snprintf(buf, sizeof buf,
                     "[handler] SIGUSR1 from sender pid=%d uid=%u\n",
                     info->si_pid, info->si_uid);
    (void)!write(STDOUT_FILENO, buf, n);
    got = 1;
}

int main(void)
{
    struct sigaction sa = {0};
    sa.sa_sigaction = on_usr1;
    sa.sa_flags     = SA_SIGINFO | SA_RESTART;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGUSR1, &sa, NULL) < 0) { perror("sigaction"); return 1; }

    printf("[main] pid=%d, send SIGUSR1 with: kill -USR1 %d\n", getpid(), getpid());

    /* Self-deliver to demonstrate */
    raise(SIGUSR1);
    while (!got) pause();

    printf("[main] handler reported sender info correctly — SA_SIGINFO works.\n");
    return 0;
}
