#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

/*
 * Task 11.3: Pass DATA through signals using sigqueue() + union sigval.
 *
 * Plain kill(pid, sig) is data-less.  sigqueue() lets the sender attach a
 * 32-bit int (or pointer in same process) that arrives in
 * siginfo_t->si_value.  This works only for REAL-TIME signals (SIGRTMIN+N),
 * NOT standard signals — standard signals coalesce, RT signals are queued.
 *
 * Demo: parent sends three SIGRTMIN values to child, child receives each
 *       distinct sival_int via the handler.
 */
static int got_count = 0;

static void on_rt(int sig, siginfo_t *info, void *ctx)
{
    (void)sig; (void)ctx;
    char buf[128];
    int n = snprintf(buf, sizeof buf,
                     "[child] got RT signal value=%d from pid=%d\n",
                     info->si_value.sival_int, info->si_pid);
    (void)!write(STDOUT_FILENO, buf, n);
    ++got_count;
}

int main(void)
{
    pid_t pid = fork();
    if (pid == 0) {
        /* child: install handler, then wait for signals */
        struct sigaction sa = {0};
        sa.sa_sigaction = on_rt;
        sa.sa_flags     = SA_SIGINFO;
        sigemptyset(&sa.sa_mask);
        sigaction(SIGRTMIN, &sa, NULL);

        while (got_count < 3) pause();
        printf("[child] received all 3 RT signals — values were queued.\n");
        _exit(0);
    }
    /* parent: send 3 distinct values */
    usleep(100000);          /* give child time to install handler */
    for (int i = 0; i < 3; ++i) {
        union sigval v;
        v.sival_int = 42 + i;
        sigqueue(pid, SIGRTMIN, v);
        usleep(10000);
    }
    wait(NULL);
    printf("[parent] all sent.\n");
    return 0;
}
