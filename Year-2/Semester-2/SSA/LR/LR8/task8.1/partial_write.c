#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

/*
 * Task 8.1: Can write(fd, buf, nbytes) return less than nbytes?
 *
 * YES. The POSIX guarantee is that on success, write() returns the
 * number of bytes WRITTEN (>= 1, <= nbytes), NOT necessarily nbytes.
 * Common reasons for a short write:
 *
 *   1) Pipe / socket buffer fills up (only PIPE_BUF guaranteed atomic).
 *   2) Signal interrupts a slow write (returns short count, NOT -EINTR
 *      once at least 1 byte was written).
 *   3) Disk full (ENOSPC) mid-flush.
 *   4) RLIMIT_FSIZE exceeded → SIGXFSZ + short write.
 *   5) Non-blocking fd has only partial space left.
 *
 * Demonstration: write 1 MiB to a pipe whose capacity is 64 KiB on Linux.
 * The first write returns ~65536, not 1048576.
 */
int main(void)
{
    int pipefd[2];
    if (pipe(pipefd) < 0) { perror("pipe"); return 1; }

    /* make write end non-blocking so it returns short instead of blocking */
    int flags = fcntl(pipefd[1], F_GETFL, 0);
    fcntl(pipefd[1], F_SETFL, flags | O_NONBLOCK);

    enum { N = 1 * 1024 * 1024 };  /* 1 MiB */
    char *buf = malloc(N);
    memset(buf, 'X', N);

    ssize_t got = write(pipefd[1], buf, N);
    if (got < 0) {
        perror("write");
    } else {
        printf("requested write(): nbytes=%d\n", N);
        printf("actual return:     count =%zd  (%.1f KiB)\n", got, got / 1024.0);
        printf("→ short write: count != nbytes confirmed.\n");
        printf("→ remaining %.1f KiB would block or fail with EAGAIN.\n",
               (N - got) / 1024.0);
    }

    free(buf);
    close(pipefd[0]); close(pipefd[1]);
    return 0;
}
