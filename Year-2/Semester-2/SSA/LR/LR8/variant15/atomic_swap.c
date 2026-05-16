#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/syscall.h>
#include <linux/fs.h>

/*
 * Variant 15 — atomic content swap of two files without creating temp files.
 *
 * The classical (NON-atomic) approach is:
 *      rename(A, tmp); rename(B, A); rename(tmp, B);
 * Two problems:
 *   1) Creates a tmp file (forbidden by the assignment).
 *   2) Not atomic — observers can see A missing or B duplicated.
 *
 * Linux 3.15+ provides renameat2() with the RENAME_EXCHANGE flag — this
 * is the ONLY way to atomically swap two paths in the kernel. It atomically
 * exchanges the two directory entries: both files keep their inodes, only
 * the name->inode mappings flip. No temp file, no observable intermediate
 * state.
 *
 * From Documentation/filesystems/api-changes.rst:
 *   "RENAME_EXCHANGE: Atomically exchange oldpath and newpath. Both paths
 *    must exist; ENOENT is returned if either does not."
 *
 * Some libc versions lack the wrapper, so we call the syscall directly.
 */
#ifndef RENAME_EXCHANGE
#define RENAME_EXCHANGE (1 << 1)
#endif

static int rename_exchange(const char *a, const char *b)
{
    return (int)syscall(SYS_renameat2, AT_FDCWD, a, AT_FDCWD, b, RENAME_EXCHANGE);
}

static int write_file(const char *path, const char *content)
{
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) return -1;
    ssize_t n = write(fd, content, strlen(content));
    close(fd);
    return n < 0 ? -1 : 0;
}

static void dump(const char *label, const char *path)
{
    char buf[256] = {0};
    int fd = open(path, O_RDONLY);
    if (fd < 0) { perror(path); return; }
    ssize_t n = read(fd, buf, sizeof buf - 1);
    if (n < 0) { perror("read"); close(fd); return; }
    buf[n] = '\0';
    printf("%s (%s): %s", label, path, buf);
    close(fd);
}

int main(int argc, char **argv)
{
    const char *A = argc > 1 ? argv[1] : "/tmp/lr8_v15_A.txt";
    const char *B = argc > 2 ? argv[2] : "/tmp/lr8_v15_B.txt";

    /* prepare two files with distinct contents */
    if (write_file(A, "I am file A\n") < 0) { perror(A); return 1; }
    if (write_file(B, "I am file B\n") < 0) { perror(B); return 1; }

    printf("Before swap:\n");
    dump("  A", A);
    dump("  B", B);

    if (rename_exchange(A, B) < 0) {
        fprintf(stderr, "renameat2(RENAME_EXCHANGE): %s\n", strerror(errno));
        fprintf(stderr, "Note: requires Linux 3.15+ and a filesystem that "
                        "supports it (ext4, btrfs, xfs — yes; tmpfs — yes).\n");
        return 1;
    }

    printf("After  renameat2(A, B, RENAME_EXCHANGE):\n");
    dump("  A", A);
    dump("  B", B);

    printf("→ The directory entries A and B atomically swapped their inodes.\n");
    printf("→ No temp file was created; no intermediate state was visible.\n");

    unlink(A);
    unlink(B);
    return 0;
}
