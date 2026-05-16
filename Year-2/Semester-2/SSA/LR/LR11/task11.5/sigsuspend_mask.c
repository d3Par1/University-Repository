#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

/*
 * Task 11.5: Atomic wait-for-signal using sigsuspend().
 *
 * Naive code has a race:
 *
 *   if (!got_signal) {
 *       pause();           // <-- signal may arrive HERE, before pause()
 *   }                      //     pause then sleeps forever!
 *
 * sigsuspend() fixes this: it atomically (a) replaces the signal mask,
 * (b) waits for a signal, (c) restores the mask. No window in between.
 *
 * Pattern: block the signal of interest, then sigsuspend with a mask that
 * UNBLOCKS it. Signal arrives → handler runs → sigsuspend returns.
 */
static volatile sig_atomic_t fired = 0;

static void on_usr1(int s) { (void)s; fired = 1; }

int main(void)
{
    struct sigaction sa = {0};
    sa.sa_handler = on_usr1;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGUSR1, &sa, NULL);

    sigset_t blocked, oldmask, waitmask;
    sigemptyset(&blocked);
    sigaddset(&blocked, SIGUSR1);
    sigprocmask(SIG_BLOCK, &blocked, &oldmask);

    /* spawn a child that will signal us after a delay */
    pid_t pid = fork();
    if (pid == 0) {
        usleep(200000);
        kill(getppid(), SIGUSR1);
        _exit(0);
    }

    printf("[main] SIGUSR1 currently blocked.  sigsuspend with empty mask...\n");
    fflush(stdout);

    /* sigsuspend with empty waitmask = "while waiting, accept ALL signals" */
    sigemptyset(&waitmask);
    sigsuspend(&waitmask);   /* atomic: unblock + wait + reblock */
    /* on return, fired must be 1 */
    printf("[main] sigsuspend returned, fired=%d\n", fired);

    sigprocmask(SIG_SETMASK, &oldmask, NULL);
    wait(NULL);
    return 0;
}
