/*
 * Task 3.2: Experimenting with resource limits using perf
 *
 * Install perf in Docker:
 *   apt-get update && apt-get install -y linux-tools-common linux-tools-generic
 *
 * Compile: gcc -Wall -Wextra -std=c11 -O2 perf_limit.c -o perf_limit
 * Run:     ./perf_limit
 * Perf:    perf stat ./perf_limit
 *          perf record ./perf_limit && perf report
 *
 * This program demonstrates hitting resource limits:
 * - Opens files until RLIMIT_NOFILE is reached
 * - Allocates memory until RLIMIT_AS is reached
 * - Uses CPU until observable by perf counters
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <signal.h>

/* Test 1: Open files until limit */
static void test_open_files_limit(void) {
    struct rlimit rl;
    getrlimit(RLIMIT_NOFILE, &rl);
    printf("[File Descriptors]\n");
    printf("  Soft limit: %lu\n", (unsigned long)rl.rlim_cur);
    printf("  Hard limit: %lu\n", (unsigned long)rl.rlim_max);

    /* Lower the soft limit for a quick test */
    rl.rlim_cur = 20;
    if (setrlimit(RLIMIT_NOFILE, &rl) != 0) {
        perror("  setrlimit NOFILE");
        return;
    }
    printf("  Set soft limit to 20\n");

    int fds[100];
    int count = 0;
    for (int i = 0; i < 100; i++) {
        fds[i] = open("/dev/null", O_RDONLY);
        if (fds[i] < 0) {
            printf("  Failed to open fd #%d: %s\n", i, strerror(errno));
            break;
        }
        count++;
    }
    printf("  Successfully opened %d file descriptors before limit\n", count);

    for (int i = 0; i < count; i++)
        close(fds[i]);
}

/* Test 2: CPU-intensive work for perf analysis */
static volatile long result = 0;

static void test_cpu_intensive(void) {
    printf("\n[CPU-Intensive Work]\n");
    printf("  Running computation for perf analysis...\n");

    long sum = 0;
    for (long i = 0; i < 100000000L; i++) {
        sum += i * i;
        if (i % 3 == 0) sum -= i;
    }
    result = sum;
    printf("  Computation done (result: %ld)\n", result);
}

/* Test 3: Memory allocation until limit */
static void test_memory_limit(void) {
    struct rlimit rl;
    getrlimit(RLIMIT_AS, &rl);
    printf("\n[Virtual Memory (RLIMIT_AS)]\n");
    printf("  Soft limit: %lu bytes\n", (unsigned long)rl.rlim_cur);

    /* Set a small virtual memory limit */
    rl.rlim_cur = 100 * 1024 * 1024; /* 100 MB */
    if (setrlimit(RLIMIT_AS, &rl) != 0) {
        perror("  setrlimit AS");
        return;
    }
    printf("  Set virtual memory limit to 100 MB\n");

    size_t total = 0;
    int blocks = 0;
    while (1) {
        void *p = malloc(1024 * 1024); /* 1 MB */
        if (!p) {
            printf("  malloc failed after %d MB: %s\n", blocks, strerror(errno));
            break;
        }
        memset(p, 0xAA, 1024 * 1024); /* Touch pages */
        total += 1024 * 1024;
        blocks++;
        /* Don't free - intentionally exhaust memory */
    }
    printf("  Allocated %zu bytes (%d MB) before limit\n", total, blocks);
}

int main(void) {
    printf("=== Task 3.2: Resource limits with perf ===\n\n");
    printf("PID: %d\n\n", getpid());

    test_open_files_limit();
    test_cpu_intensive();
    test_memory_limit();

    printf("\n=== Done ===\n");
    printf("Run with: perf stat ./perf_limit\n");
    printf("For detailed: perf record -g ./perf_limit && perf report\n");
    return 0;
}
