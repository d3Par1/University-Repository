#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/signalfd.h>
#include <sys/wait.h>

/*
 * Task 11.4: signalfd() — receive signals as readable bytes on a file
 * descriptor.  This eliminates the entire class of "async-signal-safe"
 * problems because the receiver is a normal blocking/polling read loop,
 * not a signal handler.
 *
 * Pipeline:
 *   1) Block the signal in the process mask (sigprocmask).
 *   2) Create signalfd for that mask.
 *   3) read() returns struct signalfd_siginfo per delivered signal.
 *
 * Now you can integrate with poll/epoll for unified event loops — this is
 * what systemd, sd-event, libevent, and nginx use.
 */
int main(void)
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGTERM);
    /* block them so default disposition can't kick in */
    sigprocmask(SIG_BLOCK, &mask, NULL);

    int sfd = signalfd(-1, &mask, 0);
    if (sfd < 0) { perror("signalfd"); return 1; }

    /* spawn a child that fires a few signals at us */
    pid_t pid = fork();
    if (pid == 0) {
        usleep(100000); kill(getppid(), SIGUSR1);
        usleep(100000); kill(getppid(), SIGUSR1);
        usleep(100000); kill(getppid(), SIGTERM);
        _exit(0);
    }

    int seen = 0;
    while (seen < 3) {
        struct signalfd_siginfo si;
        ssize_t n = read(sfd, &si, sizeof si);
        if (n != sizeof si) { perror("read"); break; }
        printf("[main] signalfd got signo=%u (%s) from pid=%u\n",
               si.ssi_signo,
               si.ssi_signo == SIGINT  ? "SIGINT"  :
               si.ssi_signo == SIGUSR1 ? "SIGUSR1" :
               si.ssi_signo == SIGTERM ? "SIGTERM" : "?",
               si.ssi_pid);
        ++seen;
    }
    wait(NULL);
    close(sfd);
    return 0;
}
