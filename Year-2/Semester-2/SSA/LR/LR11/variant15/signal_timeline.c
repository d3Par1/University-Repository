#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/signalfd.h>

/*
 * Variant 15 — Controller process reacts to signals from several senders,
 * sorts events by arrival time, builds an ASCII timeline graph.
 *
 * Architecture:
 *   - controller is the parent process. It forks N senders.
 *   - each sender sleeps a different fraction of a second, then
 *     sigqueue() to parent with SIGRTMIN + (its index) and sival_int = N.
 *   - controller reads events via signalfd() — events are NAMED with PID
 *     and timestamped on arrival.
 *   - after all senders finish, controller sorts by arrival time and
 *     prints an ASCII timeline.
 *
 * signalfd is used (over a sigaction handler) so we get a deterministic
 * blocking read loop and a precise timestamp without async-signal-safety
 * worries.
 */

#define N_SENDERS 6
#define MAX_EVENTS 256

typedef struct {
    double t;          /* relative time since start, seconds */
    int    rt_offset;  /* signal number - SIGRTMIN */
    pid_t  sender;
    int    payload;
} Event;

static int cmp_event(const void *a, const void *b)
{
    double da = ((const Event *)a)->t;
    double db = ((const Event *)b)->t;
    return (da > db) - (da < db);
}

static double now_s(double t0)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec + ts.tv_nsec / 1e9) - t0;
}

static double t0_global;

static void print_timeline(const Event *ev, int n)
{
    if (n == 0) return;
    double tmin = ev[0].t, tmax = ev[n - 1].t;
    if (tmax - tmin < 0.001) tmax = tmin + 0.001;

    enum { W = 50 };
    char line[W + 1];
    memset(line, '-', W); line[W] = '\0';

    puts("\nTimeline (sorted by arrival time):");
    printf("  %.3fs %.*s %.3fs\n", tmin, W, line, tmax);
    for (int i = 0; i < n; ++i) {
        double frac = (ev[i].t - tmin) / (tmax - tmin);
        int col = (int)(frac * (W - 1));
        if (col < 0) col = 0;
        if (col >= W) col = W - 1;
        char bar[W + 1]; memset(bar, ' ', W); bar[W] = '\0';
        bar[col] = '*';
        printf("  %6.3fs |%s|  rt+%d  from pid=%d  val=%d\n",
               ev[i].t, bar, ev[i].rt_offset, ev[i].sender, ev[i].payload);
    }
}

int main(void)
{
    /* Block all SIGRTMIN..SIGRTMIN+N so signalfd catches them */
    sigset_t mask;
    sigemptyset(&mask);
    for (int i = 0; i < N_SENDERS; ++i) sigaddset(&mask, SIGRTMIN + i);
    sigprocmask(SIG_BLOCK, &mask, NULL);

    int sfd = signalfd(-1, &mask, 0);
    if (sfd < 0) { perror("signalfd"); return 1; }

    /* Anchor zero */
    struct timespec ts0;
    clock_gettime(CLOCK_MONOTONIC, &ts0);
    t0_global = ts0.tv_sec + ts0.tv_nsec / 1e9;

    /* Spawn N senders with varied delays.
     * Delays are deliberately NOT monotone in sender index, so the
     * arrival order will differ from spawn order. */
    int delays_ms[N_SENDERS] = {300, 50, 200, 100, 400, 150};

    pid_t parent = getpid();
    for (int i = 0; i < N_SENDERS; ++i) {
        pid_t p = fork();
        if (p == 0) {
            usleep((useconds_t)delays_ms[i] * 1000);
            union sigval v = { .sival_int = i * 10 };
            sigqueue(parent, SIGRTMIN + i, v);
            _exit(0);
        }
    }
    printf("[controller] spawned %d senders, waiting for events...\n", N_SENDERS);

    /* Receive events */
    Event events[MAX_EVENTS];
    int count = 0;
    while (count < N_SENDERS) {
        struct signalfd_siginfo si;
        ssize_t n = read(sfd, &si, sizeof si);
        if (n != sizeof si) break;
        if (count >= MAX_EVENTS) break;
        events[count].t         = now_s(t0_global);
        events[count].rt_offset = si.ssi_signo - SIGRTMIN;
        events[count].sender    = si.ssi_pid;
        events[count].payload   = si.ssi_int;
        printf("[controller] recv rt+%d from pid=%d val=%d at %.3fs\n",
               events[count].rt_offset, events[count].sender,
               events[count].payload, events[count].t);
        ++count;
    }
    /* reap senders */
    while (wait(NULL) > 0) ;

    /* Sort by arrival time */
    qsort(events, count, sizeof(Event), cmp_event);
    print_timeline(events, count);

    close(sfd);
    return 0;
}
