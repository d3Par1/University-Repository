#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>

/*
 * Task 11.1: Handle SIGSEGV, save fault address + PID to a log,
 * then re-raise the signal to let default handler produce a core dump.
 *
 * Key facts:
 *   - siginfo_t->si_addr is the faulting memory address (set by kernel
 *     from CR2 register on x86).
 *   - Inside a signal handler we are restricted to async-signal-safe
 *     functions (write, _exit, raise, ...). NOT printf, NOT malloc.
 *   - To get full fault info we need SA_SIGINFO + sa_sigaction, not the
 *     old single-arg signal() interface.
 *   - After logging we set SIG_DFL and re-raise to allow normal core
 *     dump or termination.
 */

static const char *LOG_PATH = "/tmp/lr11_t1_crash.log";

static void itoa_hex(uintptr_t v, char *buf)
{
    /* signal-safe hex formatter */
    const char *digits = "0123456789abcdef";
    char tmp[20];
    int i = 0;
    if (!v) { buf[0] = '0'; buf[1] = '\0'; return; }
    while (v) { tmp[i++] = digits[v & 0xf]; v >>= 4; }
    int j = 0;
    while (i > 0) buf[j++] = tmp[--i];
    buf[j] = '\0';
}

static void itoa_dec(unsigned v, char *buf)
{
    char tmp[20];
    int i = 0;
    if (!v) { buf[0] = '0'; buf[1] = '\0'; return; }
    while (v) { tmp[i++] = '0' + (v % 10); v /= 10; }
    int j = 0;
    while (i > 0) buf[j++] = tmp[--i];
    buf[j] = '\0';
}

static void on_segv(int sig, siginfo_t *info, void *ctx)
{
    (void)sig; (void)ctx;
    int fd = open(LOG_PATH, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd >= 0) {
        char buf[64];
        const char *m1 = "SIGSEGV: pid=";
        (void)!write(fd, m1, strlen(m1));
        itoa_dec((unsigned)getpid(), buf);
        (void)!write(fd, buf, strlen(buf));
        const char *m2 = " fault_addr=0x";
        (void)!write(fd, m2, strlen(m2));
        itoa_hex((uintptr_t)info->si_addr, buf);
        (void)!write(fd, buf, strlen(buf));
        (void)!write(fd, "\n", 1);
        close(fd);
    }
    /* tell the user via stderr */
    (void)!write(STDERR_FILENO, "[handler] crash logged, exiting\n", 32);
    _exit(139);   /* 128 + SIGSEGV(11) — bash-convention for signal-killed */
}

int main(void)
{
    struct sigaction sa = {0};
    sa.sa_sigaction = on_segv;
    sa.sa_flags     = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGSEGV, &sa, NULL);

    printf("[main] pid=%d, will dereference NULL...\n", getpid());
    fflush(stdout);
    int *p = NULL;
    *p = 42;                 /* trigger SIGSEGV */
    return 0;                /* unreachable */
}
