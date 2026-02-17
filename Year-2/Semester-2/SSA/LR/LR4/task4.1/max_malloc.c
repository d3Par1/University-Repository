/*
 * Task 4.1: How much memory can malloc(3) allocate in a single call?
 *
 * Explores the maximum allocation size on 32-bit and 64-bit platforms.
 * sizeof(size_t) = 8 on 64-bit = 64 bits => max = 2^64 - 1
 * But virtual address space is limited to 2^48 on x86_64 (256 TB).
 *
 * Why 8 exabytes and not 16? Because size_t is unsigned,
 * max = 2^64 - 1 = ~18.4 EB. But malloc internally may use
 * signed ptrdiff_t (max 2^63 - 1 = ~8 EB), and the kernel
 * limits virtual address space to far less.
 *
 * Compile (64-bit): gcc -Wall -Wextra -std=c11 max_malloc.c -o max_malloc
 * Compile (32-bit): gcc -m32 -Wall -Wextra -std=c11 max_malloc.c -o max_malloc32
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

static const char *format_bytes(size_t bytes) {
    static char buf[64];
    if (bytes >= (size_t)1 << 40)
        snprintf(buf, sizeof(buf), "%.2f TB", (double)bytes / ((size_t)1 << 40));
    else if (bytes >= (size_t)1 << 30)
        snprintf(buf, sizeof(buf), "%.2f GB", (double)bytes / ((size_t)1 << 30));
    else if (bytes >= (size_t)1 << 20)
        snprintf(buf, sizeof(buf), "%.2f MB", (double)bytes / ((size_t)1 << 20));
    else if (bytes >= (size_t)1 << 10)
        snprintf(buf, sizeof(buf), "%.2f KB", (double)bytes / ((size_t)1 << 10));
    else
        snprintf(buf, sizeof(buf), "%zu bytes", bytes);
    return buf;
}

int main(void) {
    printf("=== Task 4.1: Maximum malloc allocation ===\n\n");

    printf("Platform info:\n");
    printf("  sizeof(void*)  = %zu bytes (%zu bits)\n", sizeof(void *), sizeof(void *) * 8);
    printf("  sizeof(size_t) = %zu bytes (%zu bits)\n", sizeof(size_t), sizeof(size_t) * 8);
    printf("  SIZE_MAX = %zu (%s)\n", SIZE_MAX, format_bytes(SIZE_MAX));
    printf("\n");

    printf("Theoretical max malloc(SIZE_MAX) = %s\n", format_bytes(SIZE_MAX));
    printf("\n");

    /* Explanation: why 8 EB not 16 EB on 64-bit */
    printf("Why ~8 EB and not ~16 EB on 64-bit?\n");
    printf("  size_t is unsigned: max = 2^64 - 1 = ~18.4 EB\n");
    printf("  But ptrdiff_t (signed): max = 2^63 - 1 = ~8 EB\n");
    printf("  Pointer arithmetic uses ptrdiff_t, so practical limit is ~8 EB\n");
    printf("  Also, x86_64 only uses 48-bit virtual addresses (256 TB)\n");
    printf("  Linux kernel reserves upper half => user space ~128 TB\n\n");

    /* Binary search for actual max allocation */
    printf("Searching for actual maximum allocation...\n");

    size_t lo = 0;
    size_t hi = SIZE_MAX;
    size_t max_success = 0;

    /* First, find approximate range with exponential search */
    size_t test_size = 1024; /* Start at 1 KB */
    while (test_size > 0 && test_size < SIZE_MAX / 2) {
        void *p = malloc(test_size);
        if (p) {
            free(p);
            max_success = test_size;
            test_size *= 2;
        } else {
            hi = test_size;
            lo = max_success;
            break;
        }
    }

    /* Binary search in [lo, hi] */
    while (lo + (1UL << 20) < hi) { /* Precision: 1 MB */
        size_t mid = lo + (hi - lo) / 2;
        void *p = malloc(mid);
        if (p) {
            free(p);
            lo = mid;
            max_success = mid;
        } else {
            hi = mid;
        }
    }

    printf("Maximum successful malloc: %zu bytes (%s)\n",
           max_success, format_bytes(max_success));

    /* Try SIZE_MAX */
    printf("\nTrying malloc(SIZE_MAX)...\n");
    void *p = malloc(SIZE_MAX);
    if (p) {
        printf("  malloc(SIZE_MAX) succeeded! (unlikely)\n");
        free(p);
    } else {
        printf("  malloc(SIZE_MAX) returned NULL (expected)\n");
    }

    /* Try half of SIZE_MAX */
    printf("Trying malloc(SIZE_MAX/2)...\n");
    p = malloc(SIZE_MAX / 2);
    if (p) {
        printf("  malloc(SIZE_MAX/2) succeeded!\n");
        free(p);
    } else {
        printf("  malloc(SIZE_MAX/2) returned NULL\n");
    }

    return 0;
}
