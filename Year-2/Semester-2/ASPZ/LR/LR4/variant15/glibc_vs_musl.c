/*
 * Variant 15: Investigate the difference between malloc in glibc and musl
 *
 * This program benchmarks and explores differences between glibc and musl
 * malloc implementations. Run it on two containers:
 *   - glibc: docker run -it ubuntu bash
 *   - musl:  docker run -it alpine sh
 *
 * Key differences:
 * 1. glibc (ptmalloc2): Thread caches, arenas, bins (fast/small/large/unsorted)
 * 2. musl (mallocng): Simpler design, group-based, no arenas
 *
 * Compile (glibc): gcc -Wall -Wextra -std=c11 -O2 -pthread glibc_vs_musl.c -o malloc_bench -lrt
 * Compile (musl):  musl-gcc -Wall -Wextra -std=c11 -O2 -pthread glibc_vs_musl.c -o malloc_bench_musl -lrt
 *                  (or compile statically inside Alpine)
 *
 * Run: ./malloc_bench
 *      ltrace ./malloc_bench 2>&1 | tail -20
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <stddef.h>

#define SMALL_SIZE    32
#define MEDIUM_SIZE   512
#define LARGE_SIZE    (64 * 1024)
#define HUGE_SIZE     (1024 * 1024)
#define ITERATIONS    100000
#define NUM_THREADS   4

/* Get high-resolution time in nanoseconds */
static long long get_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

/* ---- Test 1: Sequential malloc/free benchmark ---- */
static void bench_malloc_free(const char *label, size_t size, int count) {
    long long start = get_ns();

    for (int i = 0; i < count; i++) {
        void *p = malloc(size);
        if (p) {
            ((char *)p)[0] = 'A'; /* Touch first byte */
            free(p);
        }
    }

    long long elapsed = get_ns() - start;
    printf("  %-20s: %lld ns total, %.1f ns/op\n",
           label, elapsed, (double)elapsed / count);
}

/* ---- Test 2: Batch allocate then free ---- */
static void bench_batch(const char *label, size_t size, int count) {
    void **ptrs = malloc((size_t)count * sizeof(void *));
    if (!ptrs) return;

    long long start = get_ns();

    /* Allocate all */
    for (int i = 0; i < count; i++) {
        ptrs[i] = malloc(size);
        if (ptrs[i])
            ((char *)ptrs[i])[0] = 'B';
    }

    /* Free all */
    for (int i = 0; i < count; i++)
        free(ptrs[i]);

    long long elapsed = get_ns() - start;
    printf("  %-20s: %lld ns total, %.1f ns/op\n",
           label, elapsed, (double)elapsed / count);

    free(ptrs);
}

/* ---- Test 3: Reverse-order free ---- */
static void bench_reverse_free(const char *label, size_t size, int count) {
    void **ptrs = malloc((size_t)count * sizeof(void *));
    if (!ptrs) return;

    long long start = get_ns();

    for (int i = 0; i < count; i++) {
        ptrs[i] = malloc(size);
        if (ptrs[i])
            ((char *)ptrs[i])[0] = 'C';
    }

    /* Free in reverse order */
    for (int i = count - 1; i >= 0; i--)
        free(ptrs[i]);

    long long elapsed = get_ns() - start;
    printf("  %-20s: %lld ns total, %.1f ns/op\n",
           label, elapsed, (double)elapsed / count);

    free(ptrs);
}

/* ---- Test 4: Fragmentation test ---- */
static void bench_fragmentation(int count) {
    printf("\n--- Test 4: Fragmentation ---\n");

    void **ptrs = malloc((size_t)count * sizeof(void *));
    if (!ptrs) return;

    /* Allocate mixed sizes */
    for (int i = 0; i < count; i++) {
        size_t sz = (i % 3 == 0) ? SMALL_SIZE :
                    (i % 3 == 1) ? MEDIUM_SIZE : LARGE_SIZE;
        ptrs[i] = malloc(sz);
    }

    /* Free every other allocation */
    for (int i = 0; i < count; i += 2)
        free(ptrs[i]);

    /* Try to allocate medium chunks in the gaps */
    long long start = get_ns();
    int success = 0, fail = 0;
    for (int i = 0; i < count / 2; i++) {
        void *p = malloc(MEDIUM_SIZE);
        if (p) { success++; free(p); }
        else fail++;
    }
    long long elapsed = get_ns() - start;

    printf("  Reuse attempts: %d success, %d fail, %lld ns total\n",
           success, fail, elapsed);

    /* Clean up remaining */
    for (int i = 1; i < count; i += 2)
        free(ptrs[i]);
    free(ptrs);
}

/* ---- Test 5: Multithreaded malloc ---- */
struct thread_args {
    int thread_id;
    int count;
    size_t size;
    long long elapsed_ns;
};

static void *thread_malloc_worker(void *arg) {
    struct thread_args *ta = (struct thread_args *)arg;
    long long start = get_ns();

    for (int i = 0; i < ta->count; i++) {
        void *p = malloc(ta->size);
        if (p) {
            ((char *)p)[0] = 'T';
            free(p);
        }
    }

    ta->elapsed_ns = get_ns() - start;
    return NULL;
}

static void bench_threaded(size_t size, int per_thread) {
    printf("\n--- Test 5: Multithreaded malloc (%d threads, %d ops each) ---\n",
           NUM_THREADS, per_thread);

    pthread_t threads[NUM_THREADS];
    struct thread_args args[NUM_THREADS];

    long long start = get_ns();

    for (int i = 0; i < NUM_THREADS; i++) {
        args[i].thread_id = i;
        args[i].count = per_thread;
        args[i].size = size;
        args[i].elapsed_ns = 0;
        pthread_create(&threads[i], NULL, thread_malloc_worker, &args[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++)
        pthread_join(threads[i], NULL);

    long long total = get_ns() - start;

    for (int i = 0; i < NUM_THREADS; i++)
        printf("  Thread %d: %lld ns (%.1f ns/op)\n",
               i, args[i].elapsed_ns, (double)args[i].elapsed_ns / per_thread);
    printf("  Wall time: %lld ns (%.1f ns/op total)\n",
           total, (double)total / (NUM_THREADS * per_thread));
}

/* ---- Test 6: malloc(0) and edge cases ---- */
static void test_edge_cases(void) {
    printf("\n--- Test 6: Edge cases ---\n");

    /* malloc(0) */
    void *p0 = malloc(0);
    printf("  malloc(0)    = %p\n", p0);
    free(p0);

    /* realloc(NULL, 0) */
    void *pn = realloc(NULL, 0);
    printf("  realloc(NULL, 0) = %p\n", pn);
    free(pn);

    /* realloc(ptr, 0) */
    void *pp = malloc(100);
    void *pr = realloc(pp, 0);
    printf("  realloc(malloc(100), 0) = %p\n", pr);
    if (pr) free(pr);
    /* If pr is NULL, pp was freed by realloc (glibc behavior) */
    /* If pr is non-NULL, pp may still need freeing (musl behavior) */

    /* Alignment check */
    printf("  Alignment test:\n");
    for (int i = 0; i < 5; i++) {
        void *p = malloc((size_t)(1 << i));
        printf("    malloc(%d) = %p (aligned to %zu)\n",
               1 << i, p, (size_t)p % 16);
        free(p);
    }
}

/* ---- Test 7: Memory overhead estimation ---- */
static void test_overhead(void) {
    printf("\n--- Test 7: Memory overhead per allocation ---\n");
    printf("  (Estimated by spacing between consecutive allocations)\n");

    size_t sizes[] = {1, 8, 16, 24, 32, 48, 64, 128, 256, 512, 1024};
    int nsizes = sizeof(sizes) / sizeof(sizes[0]);

    for (int s = 0; s < nsizes; s++) {
        void *p1 = malloc(sizes[s]);
        void *p2 = malloc(sizes[s]);
        if (p1 && p2) {
            ptrdiff_t diff = (char *)p2 - (char *)p1;
            if (diff < 0) diff = -diff;
            printf("    malloc(%3zu): spacing = %td bytes (overhead ~%td)\n",
                   sizes[s], diff, diff - (ptrdiff_t)sizes[s]);
        }
        free(p2);
        free(p1);
    }
}

int main(void) {
    printf("=== Variant 15: glibc vs musl malloc comparison ===\n\n");

    /* Detect which libc we're using */
    #ifdef __GLIBC__
    printf("C library: glibc %d.%d\n", __GLIBC__, __GLIBC_MINOR__);
    printf("Allocator: ptmalloc2 (based on dlmalloc)\n");
    #else
    printf("C library: non-glibc (likely musl)\n");
    printf("Allocator: mallocng (musl) or other\n");
    #endif
    printf("sizeof(void*) = %zu\n\n", sizeof(void *));

    /* Test 1: Sequential malloc/free */
    printf("--- Test 1: Sequential malloc/free (%d iterations) ---\n", ITERATIONS);
    bench_malloc_free("Small (32B)",  SMALL_SIZE,  ITERATIONS);
    bench_malloc_free("Medium (512B)", MEDIUM_SIZE, ITERATIONS);
    bench_malloc_free("Large (64KB)", LARGE_SIZE,  ITERATIONS / 10);
    bench_malloc_free("Huge (1MB)",   HUGE_SIZE,   ITERATIONS / 100);

    /* Test 2: Batch allocate then free (FIFO order) */
    printf("\n--- Test 2: Batch alloc+free (FIFO) ---\n");
    bench_batch("Small (32B)",  SMALL_SIZE,  ITERATIONS / 10);
    bench_batch("Medium (512B)", MEDIUM_SIZE, ITERATIONS / 10);

    /* Test 3: Batch allocate, free in reverse (LIFO) */
    printf("\n--- Test 3: Batch alloc+free (LIFO) ---\n");
    bench_reverse_free("Small (32B)",  SMALL_SIZE,  ITERATIONS / 10);
    bench_reverse_free("Medium (512B)", MEDIUM_SIZE, ITERATIONS / 10);

    /* Test 4: Fragmentation */
    bench_fragmentation(10000);

    /* Test 5: Multithreaded */
    bench_threaded(SMALL_SIZE, ITERATIONS / 10);

    /* Test 6: Edge cases */
    test_edge_cases();

    /* Test 7: Overhead */
    test_overhead();

    /* Summary of known differences */
    printf("\n=== Key Differences: glibc vs musl malloc ===\n\n");
    printf("Feature              | glibc (ptmalloc2)         | musl (mallocng)\n");
    printf("---------------------|---------------------------|---------------------------\n");
    printf("Design               | Arena-based, thread cache | Group-based, simpler\n");
    printf("Thread safety        | Per-arena locks           | Global lock (fine-grained)\n");
    printf("Small alloc          | Fastbins (LIFO, no lock)  | Size-class groups\n");
    printf("Large alloc          | mmap threshold (128KB)    | mmap for large blocks\n");
    printf("Alignment            | 16 bytes (64-bit)         | 16 bytes (64-bit)\n");
    printf("Overhead per alloc   | 16-32 bytes               | 16 bytes typically\n");
    printf("malloc(0)            | Returns non-NULL (min 32) | Returns non-NULL (min 16)\n");
    printf("realloc(p,0)         | Frees, returns NULL       | Returns non-NULL\n");
    printf("Memory return to OS  | Uses madvise/MADV_DONTNEED| Unmaps via munmap\n");
    printf("Fragmentation        | Can be high (arenas)      | Generally lower\n");
    printf("Code size            | ~20K lines                | ~1.5K lines\n");
    printf("Debug features       | MALLOC_CHECK_, mtrace     | None built-in\n");
    printf("Thread scalability   | Better (per-arena)        | Limited (single lock)\n");
    printf("\n");
    printf("To compare, compile statically with musl:\n");
    printf("  # On Alpine Linux (uses musl natively):\n");
    printf("  gcc -static -O2 -pthread glibc_vs_musl.c -o bench_musl -lrt\n");
    printf("  # On Ubuntu (install musl-tools):\n");
    printf("  apt install musl-tools && musl-gcc -static -O2 -pthread glibc_vs_musl.c -o bench_musl -lrt\n");
    printf("  # Then compare:\n");
    printf("  ./bench_glibc > glibc_results.txt\n");
    printf("  ./bench_musl  > musl_results.txt\n");
    printf("  diff glibc_results.txt musl_results.txt\n");

    return 0;
}
