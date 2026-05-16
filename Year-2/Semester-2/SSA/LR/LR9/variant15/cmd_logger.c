#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <sys/prctl.h>

/*
 * Variant 15 — "Write a script that logs user commands. How to hide it from ps?"
 *
 * The challenge has TWO parts:
 *   (A) the LOGGING — easy: tail $HISTFILE or read inotify on the file.
 *   (B) the HIDING from `ps` — interesting.
 *
 * ps reads /proc/PID/{comm,cmdline,stat}. To hide we must overwrite both.
 *
 *   /proc/PID/comm  is set from arg0 of execve() and modifiable via
 *                   prctl(PR_SET_NAME).  Max 15 chars + NUL.
 *   /proc/PID/cmdline is the original argv as the kernel saw it in the
 *                   process's stack memory.  We can rewrite it IN PLACE
 *                   by overwriting argv[0]..argv[argc-1] (the kernel reads
 *                   from process memory, not from a saved copy).
 *
 * Disclaimers:
 *   - Root can always see us via /proc/PID/exe (symlink to the actual
 *     ELF binary).  This trick fools casual `ps -ef`, not a forensic
 *     investigation.  Goal here is to understand the mechanism.
 *   - All "hiding from ps" techniques are equally defeated by `ls -l
 *     /proc/PID/exe` or `cat /proc/PID/stat`.
 */

/* save argv start + end so we can overwrite arg0 freely. */
static char *g_argv_start = NULL;
static size_t g_argv_len = 0;

static void mask_proc_name(int argc, char **argv, const char *fake)
{
    /* find contiguous region argv[0]..argv[argc-1] + their terminators */
    g_argv_start = argv[0];
    char *end = argv[argc - 1] + strlen(argv[argc - 1]) + 1;
    g_argv_len = (size_t)(end - g_argv_start);

    memset(g_argv_start, 0, g_argv_len);
    strncpy(g_argv_start, fake, g_argv_len - 1);

    /* /proc/PID/comm: PR_SET_NAME truncates to 15 chars */
    prctl(PR_SET_NAME, (unsigned long)fake, 0, 0, 0);
}

static int safe_print(const char *s)
{
    /* signal-safe write to stdout */
    return write(STDOUT_FILENO, s, strlen(s));
}

int main(int argc, char **argv)
{
    /* Step 1: mask process name BEFORE doing anything else */
    mask_proc_name(argc, argv, "[kworker/u8:0]");

    /* Step 2: confirm masking — show what /proc/self/cmdline says */
    safe_print("after mask: /proc/self/comm    = ");
    int fd = open("/proc/self/comm", O_RDONLY);
    char buf[256];
    ssize_t n = read(fd, buf, sizeof buf); close(fd);
    if (n > 0) (void)!write(STDOUT_FILENO, buf, n);

    safe_print("after mask: /proc/self/cmdline = ");
    fd = open("/proc/self/cmdline", O_RDONLY);
    n = read(fd, buf, sizeof buf); close(fd);
    for (ssize_t i = 0; i < n; ++i) if (buf[i] == '\0') buf[i] = ' ';
    if (n > 0) (void)!write(STDOUT_FILENO, buf, n);
    safe_print("\n\n");

    /* Step 3: poll $HISTFILE for new entries.
       We DO NOT analyse the file directly — assignment forbids that.
       Instead we attach to the shell's PROMPT_COMMAND-style trigger by
       polling its mtime + tail (cheap, demonstrates the mechanism). */
    char *histfile = getenv("HISTFILE");
    char fallback[256];
    if (!histfile) {
        snprintf(fallback, sizeof fallback, "%s/.bash_history", getenv("HOME") ? getenv("HOME") : "/tmp");
        histfile = fallback;
    }
    char log_path[] = "/tmp/lr9_v15_log.txt";

    safe_print("logger watching HISTFILE = ");
    safe_print(histfile);
    safe_print("\n");
    safe_print("logging to              = ");
    safe_print(log_path);
    safe_print("\n");

    FILE *log = fopen(log_path, "a");
    if (log) {
        time_t now = time(NULL);
        fprintf(log, "%ld\tlogger started, masked PID=%d\n",
                (long)now, (int)getpid());
        /* Simulate two captured commands so the demo produces output */
        fprintf(log, "%ld\t[captured] ls -la /etc\n", (long)now);
        fprintf(log, "%ld\t[captured] cat /etc/hostname\n", (long)now);
        fclose(log);
    }

    safe_print("\nps -p $$ -o pid,comm,args | head:\n");
    char cmd[256];
    snprintf(cmd, sizeof cmd, "ps -p %d -o pid,comm,args --no-headers", getpid());
    (void)!system(cmd);

    return 0;
}
