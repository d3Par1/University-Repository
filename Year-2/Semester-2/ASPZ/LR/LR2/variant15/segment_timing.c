/*
 * Variant 15 - Measure memory access time for different segments
 *
 * Measures read/write latency for:
 *   - Text segment (code, read-only)
 *   - Data segment (initialized globals)
 *   - BSS segment (uninitialized globals)
 *   - Heap (malloc)
 *   - Stack (local variables)
 *   - mmap'd memory
 *
 * Compile: gcc -Wall -O0 -o variant15 segment_timing.c -lrt
 *   (use -O0 to prevent compiler from optimizing away accesses)
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>
#include <stdint.h>

#define ITERATIONS 100000000
#define ARRAY_SIZE (1024 * 1024)  /* 1M ints = 4MB */

/* Data segment (initialized) */
int data_array[ARRAY_SIZE] = {1, 2, 3};

/* BSS segment (uninitialized) */
int bss_array[ARRAY_SIZE];

/* Get time in nanoseconds */
static inline uint64_t get_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

/* Prevent compiler from optimizing away */
volatile int sink;

void measure_sequential(const char *name, volatile int *arr, int size) {
    uint64_t start = get_ns();
    int sum = 0;
    for (int iter = 0; iter < 10; iter++) {
        for (int i = 0; i < size; i++) {
            sum += arr[i];
        }
    }
    uint64_t elapsed = get_ns() - start;
    sink = sum;

    double ns_per_access = (double)elapsed / (10.0 * size);
    printf("  %-20s: %8.2f ns/access  (total %.3f ms, addr: %p)\n",
           name, ns_per_access, elapsed / 1e6, (const void *)arr);
}

void measure_random(const char *name, volatile int *arr, int size) {
    /* Pre-generate random indices */
    int *indices = malloc(size * sizeof(int));
    for (int i = 0; i < size; i++) indices[i] = rand() % size;

    uint64_t start = get_ns();
    int sum = 0;
    for (int i = 0; i < size; i++) {
        sum += arr[indices[i]];
    }
    uint64_t elapsed = get_ns() - start;
    sink = sum;

    double ns_per_access = (double)elapsed / size;
    printf("  %-20s: %8.2f ns/access  (total %.3f ms)\n",
           name, ns_per_access, elapsed / 1e6);
    free(indices);
}

void measure_write(const char *name, volatile int *arr, int size) {
    uint64_t start = get_ns();
    for (int iter = 0; iter < 10; iter++) {
        for (int i = 0; i < size; i++) {
            arr[i] = i;
        }
    }
    uint64_t elapsed = get_ns() - start;

    double ns_per_access = (double)elapsed / (10.0 * size);
    printf("  %-20s: %8.2f ns/access  (total %.3f ms)\n",
           name, ns_per_access, elapsed / 1e6);
}

int main() {
    printf("=== Variant 15: Memory Segment Access Time ===\n");
    printf("Array size: %d ints (%d MB)\n\n", ARRAY_SIZE,
           (int)(ARRAY_SIZE * sizeof(int) / (1024 * 1024)));

    srand(42);

    /* Stack segment */
    int stack_array[ARRAY_SIZE / 4]; /* Smaller to avoid stack overflow */
    memset(stack_array, 0, sizeof(stack_array));

    /* Heap segment */
    int *heap_array = malloc(ARRAY_SIZE * sizeof(int));
    memset(heap_array, 0, ARRAY_SIZE * sizeof(int));

    /* mmap segment */
    int *mmap_array = mmap(NULL, ARRAY_SIZE * sizeof(int),
                           PROT_READ | PROT_WRITE,
                           MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    memset(mmap_array, 0, ARRAY_SIZE * sizeof(int));

    /* Warm up caches */
    for (int i = 0; i < ARRAY_SIZE; i++) {
        data_array[i] = i;
        bss_array[i] = i;
        heap_array[i] = i;
        mmap_array[i] = i;
    }
    for (int i = 0; i < ARRAY_SIZE / 4; i++) stack_array[i] = i;

    /* === Sequential Read === */
    printf("--- Sequential Read ---\n");
    measure_sequential("DATA segment", data_array, ARRAY_SIZE);
    measure_sequential("BSS segment", bss_array, ARRAY_SIZE);
    measure_sequential("HEAP (malloc)", heap_array, ARRAY_SIZE);
    measure_sequential("STACK", (volatile int *)stack_array, ARRAY_SIZE / 4);
    measure_sequential("MMAP", mmap_array, ARRAY_SIZE);

    /* === Random Read === */
    printf("\n--- Random Read (cache-unfriendly) ---\n");
    measure_random("DATA segment", data_array, ARRAY_SIZE);
    measure_random("BSS segment", bss_array, ARRAY_SIZE);
    measure_random("HEAP (malloc)", heap_array, ARRAY_SIZE);
    measure_random("STACK", (volatile int *)stack_array, ARRAY_SIZE / 4);
    measure_random("MMAP", mmap_array, ARRAY_SIZE);

    /* === Sequential Write === */
    printf("\n--- Sequential Write ---\n");
    measure_write("DATA segment", data_array, ARRAY_SIZE);
    measure_write("BSS segment", bss_array, ARRAY_SIZE);
    measure_write("HEAP (malloc)", heap_array, ARRAY_SIZE);
    measure_write("STACK", (volatile int *)stack_array, ARRAY_SIZE / 4);
    measure_write("MMAP", mmap_array, ARRAY_SIZE);

    /* === Address Space Map === */
    printf("\n--- Memory Map ---\n");
    printf("  TEXT  (code):    %p\n", (void *)main);
    printf("  DATA  (init):    %p\n", (void *)data_array);
    printf("  BSS   (uninit):  %p\n", (void *)bss_array);
    printf("  HEAP  (malloc):  %p\n", (void *)heap_array);
    printf("  MMAP  (anon):    %p\n", (void *)mmap_array);
    printf("  STACK (local):   %p\n", (void *)stack_array);

    /* Show /proc/self/maps for full picture */
    printf("\n--- /proc/self/maps (excerpt) ---\n");
    FILE *fp = fopen("/proc/self/maps", "r");
    if (fp) {
        char line[256];
        int count = 0;
        while (fgets(line, sizeof(line), fp) && count < 20) {
            printf("  %s", line);
            count++;
        }
        fclose(fp);
    }

    free(heap_array);
    munmap(mmap_array, ARRAY_SIZE * sizeof(int));

    printf("\n--- Analysis ---\n");
    printf("Key observations:\n");
    printf("  1. Sequential access is fast for ALL segments (cache-friendly)\n");
    printf("  2. Random access shows cache miss penalties\n");
    printf("  3. Stack, heap, data, BSS all map to same physical RAM\n");
    printf("  4. Actual performance depends on cache behavior, not segment type\n");
    printf("  5. mmap may show slightly different behavior due to page faults\n");

    return 0;
}
