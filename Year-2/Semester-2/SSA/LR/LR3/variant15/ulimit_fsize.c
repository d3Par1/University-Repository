/*
 * Variant 15: Investigate impact of ulimit -f on large file writes
 *
 * This program demonstrates how RLIMIT_FSIZE affects programs that
 * write large files. It tests different write strategies:
 * 1. Single large write
 * 2. Many small writes
 * 3. mmap-based writes
 * 4. Write with buffered I/O (fwrite)
 *
 * Compile: gcc -Wall -Wextra -std=c11 -O2 ulimit_fsize.c -o ulimit_fsize -lrt
 * Run:     ./ulimit_fsize [max_file_kb]
 * Docker:  ulimit -f 100 && ./ulimit_fsize
 *
 * The program sets RLIMIT_FSIZE and observes how different I/O methods
 * respond to the limit.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/resource.h>
#include <sys/mman.h>
#include <sys/stat.h>

static volatile sig_atomic_t fsize_exceeded = 0;

static void sigxfsz_handler(int sig) {
    (void)sig;
    fsize_exceeded = 1;
}

/* Get time in milliseconds */
static double get_time_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1000000.0;
}

/* Test 1: Write using fwrite (buffered I/O) */
static size_t test_fwrite(const char *filename, size_t chunk_size, size_t total_target) {
    fsize_exceeded = 0;
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("  fopen");
        return 0;
    }

    char *buf = malloc(chunk_size);
    if (!buf) {
        fclose(fp);
        return 0;
    }
    memset(buf, 'A', chunk_size);

    size_t written = 0;
    while (written < total_target && !fsize_exceeded) {
        size_t to_write = chunk_size;
        if (written + to_write > total_target)
            to_write = total_target - written;

        size_t ret = fwrite(buf, 1, to_write, fp);
        if (ret < to_write || fsize_exceeded) {
            written += ret;
            break;
        }
        written += ret;
        fflush(fp); /* Force kernel writes to trigger SIGXFSZ */
    }

    fclose(fp);
    free(buf);
    return written;
}

/* Test 2: Write using write() (unbuffered, system call) */
static size_t test_write_syscall(const char *filename, size_t chunk_size, size_t total_target) {
    fsize_exceeded = 0;
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("  open");
        return 0;
    }

    char *buf = malloc(chunk_size);
    if (!buf) {
        close(fd);
        return 0;
    }
    memset(buf, 'B', chunk_size);

    size_t written = 0;
    while (written < total_target && !fsize_exceeded) {
        size_t to_write = chunk_size;
        if (written + to_write > total_target)
            to_write = total_target - written;

        ssize_t ret = write(fd, buf, to_write);
        if (ret < 0 || fsize_exceeded) {
            if (errno == EFBIG || fsize_exceeded)
                break;
            perror("  write");
            break;
        }
        written += (size_t)ret;
    }

    close(fd);
    free(buf);
    return written;
}

/* Test 3: Write using mmap + msync */
static size_t test_mmap_write(const char *filename, size_t total_target) {
    fsize_exceeded = 0;
    int fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("  open");
        return 0;
    }

    /* Try to extend the file */
    if (ftruncate(fd, (off_t)total_target) != 0) {
        if (fsize_exceeded) {
            printf("  ftruncate hit file size limit\n");
            close(fd);
            return 0;
        }
        perror("  ftruncate");
        close(fd);
        return 0;
    }

    /* Map the file */
    void *map = mmap(NULL, total_target, PROT_READ | PROT_WRITE,
                     MAP_SHARED, fd, 0);
    if (map == MAP_FAILED) {
        perror("  mmap");
        close(fd);
        return 0;
    }

    /* Write data via memory */
    memset(map, 'C', total_target);
    if (msync(map, total_target, MS_SYNC) != 0) {
        perror("  msync");
    }

    size_t written = total_target;
    munmap(map, total_target);
    close(fd);
    return written;
}

/* Test 4: Single large write */
static size_t test_single_write(const char *filename, size_t total_target) {
    fsize_exceeded = 0;
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("  open");
        return 0;
    }

    char *buf = malloc(total_target);
    if (!buf) {
        close(fd);
        return 0;
    }
    memset(buf, 'D', total_target);

    ssize_t ret = write(fd, buf, total_target);
    size_t written = 0;
    if (ret > 0)
        written = (size_t)ret;

    if (fsize_exceeded) {
        printf("  SIGXFSZ received during single write\n");
    }

    close(fd);
    free(buf);
    return written;
}

static void run_test(const char *name, const char *filename,
                     size_t (*test_fn)(const char *, size_t, size_t),
                     size_t chunk, size_t target, rlim_t limit) {
    printf("\n--- %s ---\n", name);
    printf("  Chunk size: %zu bytes, Target: %zu bytes, Limit: %lu bytes\n",
           chunk, target, (unsigned long)limit);

    double start = get_time_ms();
    size_t written = test_fn(filename, chunk, target);
    double elapsed = get_time_ms() - start;

    printf("  Written: %zu bytes (%.1f%% of target)\n", written,
           target > 0 ? 100.0 * written / target : 0.0);
    printf("  Time: %.2f ms\n", elapsed);

    if (fsize_exceeded)
        printf("  ** SIGXFSZ was received **\n");

    /* Clean up test file */
    unlink(filename);
}

int main(int argc, char *argv[]) {
    rlim_t max_file_kb = 100; /* Default 100 KB */

    if (argc >= 2)
        max_file_kb = (rlim_t)atol(argv[1]);

    rlim_t max_file_bytes = max_file_kb * 1024;

    printf("=== Variant 15: ulimit -f Impact on Large File Writes ===\n\n");

    /* Install SIGXFSZ handler */
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigxfsz_handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGXFSZ, &sa, NULL) != 0) {
        perror("sigaction");
        return 1;
    }

    /* Show current file size limit */
    struct rlimit rl;
    getrlimit(RLIMIT_FSIZE, &rl);
    printf("Current RLIMIT_FSIZE:\n");
    if (rl.rlim_cur == RLIM_INFINITY)
        printf("  Soft: unlimited\n");
    else
        printf("  Soft: %lu bytes\n", (unsigned long)rl.rlim_cur);
    if (rl.rlim_max == RLIM_INFINITY)
        printf("  Hard: unlimited\n");
    else
        printf("  Hard: %lu bytes\n", (unsigned long)rl.rlim_max);

    /* Set file size limit */
    rl.rlim_cur = max_file_bytes;
    rl.rlim_max = max_file_bytes;
    if (setrlimit(RLIMIT_FSIZE, &rl) != 0) {
        perror("setrlimit FSIZE");
        return 1;
    }
    printf("\nSet RLIMIT_FSIZE to %lu KB (%lu bytes)\n",
           (unsigned long)max_file_kb, (unsigned long)max_file_bytes);

    size_t target = max_file_bytes * 2; /* Try to write 2x the limit */

    printf("\nTarget write size: %zu bytes (2x the limit)\n", target);

    /* Test 1: fwrite with small chunks */
    run_test("Test 1: fwrite (buffered, 4KB chunks)",
             "/tmp/test_fwrite.dat",
             test_fwrite, 4096, target, max_file_bytes);

    /* Test 2: fwrite with large chunks */
    run_test("Test 2: fwrite (buffered, 64KB chunks)",
             "/tmp/test_fwrite_large.dat",
             test_fwrite, 65536, target, max_file_bytes);

    /* Test 3: write() with small chunks */
    run_test("Test 3: write() syscall (4KB chunks)",
             "/tmp/test_write.dat",
             test_write_syscall, 4096, target, max_file_bytes);

    /* Test 4: write() with large chunks */
    run_test("Test 4: write() syscall (64KB chunks)",
             "/tmp/test_write_large.dat",
             test_write_syscall, 65536, target, max_file_bytes);

    /* Test 5: Single large write */
    printf("\n--- Test 5: Single large write() ---\n");
    printf("  Target: %zu bytes, Limit: %lu bytes\n",
           target, (unsigned long)max_file_bytes);
    double start = get_time_ms();
    size_t written = test_single_write("/tmp/test_single.dat", target);
    double elapsed = get_time_ms() - start;
    printf("  Written: %zu bytes (%.1f%% of target)\n", written,
           target > 0 ? 100.0 * written / target : 0.0);
    printf("  Time: %.2f ms\n", elapsed);
    if (fsize_exceeded)
        printf("  ** SIGXFSZ was received **\n");
    unlink("/tmp/test_single.dat");

    /* Test 6: mmap-based write */
    printf("\n--- Test 6: mmap + msync ---\n");
    printf("  Target: %zu bytes, Limit: %lu bytes\n",
           target, (unsigned long)max_file_bytes);
    start = get_time_ms();
    fsize_exceeded = 0;
    written = test_mmap_write("/tmp/test_mmap.dat", target);
    elapsed = get_time_ms() - start;
    printf("  Written: %zu bytes\n", written);
    printf("  Time: %.2f ms\n", elapsed);
    if (fsize_exceeded)
        printf("  ** SIGXFSZ was received **\n");
    printf("  Note: mmap may bypass RLIMIT_FSIZE in some cases\n");
    unlink("/tmp/test_mmap.dat");

    /* Summary */
    printf("\n=== Summary ===\n");
    printf("RLIMIT_FSIZE controls max file size a process can create.\n");
    printf("Key findings:\n");
    printf("  - write()/fwrite(): SIGXFSZ sent, write returns error\n");
    printf("  - Single large write: Partial write up to limit\n");
    printf("  - mmap: ftruncate may fail, or SIGBUS on page fault\n");
    printf("  - The limit is per-process, not per-file\n");
    printf("  - Use 'ulimit -f <blocks>' in shell (units of 512-byte blocks)\n");
    printf("  - Docker: set via --ulimit fsize=<soft>:<hard>\n");

    return 0;
}
