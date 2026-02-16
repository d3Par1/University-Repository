/*
 * Task 5.3: Memory leak
 *
 * Demonstrates memory leaks — allocated memory that is never freed.
 * The pointer goes out of scope but the heap block persists.
 * Over time this exhausts available memory in long-running programs.
 *
 * Compile: gcc -Wall -Wextra -std=c11 -g memory_leak.c -o memory_leak
 * Run:     ./memory_leak
 * Valgrind: valgrind --leak-check=full --show-leak-kinds=all ./memory_leak
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Simple leak: pointer goes out of scope */
static void demo_simple_leak(void) {
    printf("--- Simple leak ---\n");
    int *p = malloc(100 * sizeof(int));
    if (p) {
        p[0] = 42;
        printf("  Allocated 400 bytes, p[0] = %d\n", p[0]);
    }
    /* No free(p) — memory is leaked when function returns */
    printf("  Returning without free() — 400 bytes leaked!\n\n");
}

/* Leak in a loop: accumulates over iterations */
static void demo_loop_leak(int iterations) {
    printf("--- Loop leak (%d iterations) ---\n", iterations);
    for (int i = 0; i < iterations; i++) {
        char *buf = malloc(256);
        if (buf) {
            snprintf(buf, 256, "iteration %d", i);
            /* buf is never freed — 256 bytes leaked each iteration */
        }
    }
    printf("  Leaked %d * 256 = %d bytes total\n",
           iterations, iterations * 256);
    printf("\n");
}

/* Leak by pointer reassignment */
static void demo_reassignment_leak(void) {
    printf("--- Reassignment leak ---\n");
    int *p = malloc(64 * sizeof(int));
    printf("  First allocation: %p\n", (void *)p);

    /* Reassign without freeing — first block is lost forever */
    p = malloc(32 * sizeof(int));
    printf("  Second allocation: %p\n", (void *)p);
    printf("  First block (256 bytes) is now unreachable — leaked!\n");

    free(p); /* Only frees the second allocation */
    printf("\n");
}

/* Leak-free version for comparison */
static void demo_no_leak(void) {
    printf("--- Correct version (no leak) ---\n");
    int *p = malloc(100 * sizeof(int));
    if (p) {
        p[0] = 42;
        printf("  Allocated 400 bytes, p[0] = %d\n", p[0]);
        free(p);
        printf("  Freed 400 bytes — no leak\n");
    }
    printf("\n");
}

int main(void) {
    printf("=== Task 5.3: Memory Leaks ===\n\n");

    demo_simple_leak();
    demo_loop_leak(5);
    demo_reassignment_leak();
    demo_no_leak();

    printf("=== Summary ===\n");
    printf("Valgrind reports 'definitely lost' for unreachable blocks.\n");
    printf("Always free() every malloc()'d pointer before losing it.\n");

    return 0;
}
