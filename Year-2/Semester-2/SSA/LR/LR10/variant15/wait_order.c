#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

/*
 * Variant 15 — Spawn N children with different sleep durations.
 *              Observe how wait() returns PIDs in TERMINATION order,
 *              not spawn order.
 *
 * The lesson: wait() returns the FIRST already-zombie child it finds,
 * regardless of which fork() returned its PID first.  The order is
 * therefore determined by the scheduler, not by us.
 *
 * Output proves the inversion: child #4 sleeps 1s and is reaped first,
 * child #1 sleeps 5s and is reaped last.
 */

typedef struct {
    pid_t pid;
    int   index;
    int   sleep_s;
    double t_spawned;
    double t_reaped;
} ChildInfo;

static double now_s(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

int main(void)
{
    enum { N = 5 };
    /* Different sleep durations so order of termination != order of spawn */
    int delays[N] = {5, 4, 3, 2, 1};
    ChildInfo info[N];

    double t0 = now_s();

    /* spawn */
    for (int i = 0; i < N; ++i) {
        pid_t pid = fork();
        if (pid < 0) { perror("fork"); return 1; }
        if (pid == 0) {
            sleep(delays[i]);
            _exit(100 + i);
        }
        info[i].pid       = pid;
        info[i].index     = i + 1;
        info[i].sleep_s   = delays[i];
        info[i].t_spawned = now_s() - t0;
        info[i].t_reaped  = -1;
        printf("[parent] spawned child #%d pid=%d sleep=%ds at t=%.3f s\n",
               info[i].index, pid, delays[i], info[i].t_spawned);
    }

    puts("");

    /* reap */
    int order = 0;
    for (int reaped = 0; reaped < N; ++reaped) {
        int status;
        pid_t pid = wait(&status);
        double t = now_s() - t0;
        /* find which child this was */
        int idx = -1;
        for (int j = 0; j < N; ++j) if (info[j].pid == pid) { idx = j; break; }
        info[idx].t_reaped = t;
        printf("[parent] wait() #%d returned pid=%d (was child #%d, sleep=%ds) at t=%.3f s exit=%d\n",
               ++order, pid, info[idx].index, info[idx].sleep_s, t,
               WIFEXITED(status) ? WEXITSTATUS(status) : -1);
    }

    puts("\nSummary (spawn order vs reap order):");
    printf("  %-6s %-6s %-6s %-6s %-6s\n", "child", "pid", "sleep", "spawn", "reaped");
    for (int i = 0; i < N; ++i) {
        printf("  #%-5d %-6d %-6d %-6.2f %-6.2f\n",
               info[i].index, info[i].pid, info[i].sleep_s,
               info[i].t_spawned, info[i].t_reaped);
    }
    return 0;
}
