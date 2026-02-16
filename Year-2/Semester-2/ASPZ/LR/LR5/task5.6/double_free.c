/*
 * Task 5.6: Double free demonstration
 *
 * Freeing memory twice corrupts the heap allocator's internal
 * structures. This can lead to crashes, heap poisoning, or
 * security exploits.
 *
 * Compile: gcc -Wall -Wextra -std=c11 -g double_free.c -o double_free
 * Run:     ./double_free
 * Valgrind: valgrind ./double_free
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Demo 1: Simple double free */
static void demo_double_free(void) {
    printf("--- Demo 1: Double free ---\n");
    int *p = malloc(sizeof(int));
    if (!p) return;
    *p = 42;
    printf("  Allocated: p = %p, *p = %d\n", (void *)p, *p);

    free(p);
    printf("  First free(p) — OK\n");

    /*
     * Second free(p) — undefined behavior!
     * glibc may detect it: "double free or corruption"
     * Valgrind reports: Invalid free()
     *
     * Uncomment to see the crash:
     * free(p);
     */
    printf("  Second free(p) — would trigger: Invalid free()\n");
    printf("  glibc: 'double free or corruption (fasttop)'\n\n");
}

/* Demo 2: Double free through aliasing */
static void demo_alias_double_free(void) {
    printf("--- Demo 2: Double free through pointer aliasing ---\n");
    int *a = malloc(sizeof(int));
    int *b = a; /* b is an alias for a */

    if (!a) return;
    *a = 100;
    printf("  a = %p, b = %p (same address)\n", (void *)a, (void *)b);

    free(a);
    printf("  free(a) — OK\n");

    /*
     * free(b) is double free — b points to the same freed block.
     * This is hard to detect by code review when a and b are in
     * different functions or modules.
     *
     * Uncomment to see: free(b);
     */
    printf("  free(b) — double free! (b == a, already freed)\n\n");
}

/* Demo 3: Safe pattern — set to NULL after free */
static void demo_safe_double_free(void) {
    printf("--- Demo 3: Safe pattern with NULL check ---\n");
    int *p = malloc(sizeof(int));
    if (!p) return;
    *p = 42;

    free(p);
    p = NULL;

    /* free(NULL) is defined as a no-op by C standard */
    free(p);
    printf("  free(NULL) is safe — no-op by C standard\n");
    printf("  Always set ptr = NULL after free to prevent double free\n\n");
}

int main(void) {
    printf("=== Task 5.6: Double Free ===\n\n");

    printf("Double free: calling free() on already-freed memory.\n");
    printf("Consequences:\n");
    printf("  - Heap corruption\n");
    printf("  - Crash (glibc detects some cases)\n");
    printf("  - Security exploit (heap poisoning)\n\n");

    demo_double_free();
    demo_alias_double_free();
    demo_safe_double_free();

    printf("=== Summary ===\n");
    printf("Prevention:\n");
    printf("  1. Set ptr = NULL after every free()\n");
    printf("  2. Track ownership — only one owner frees\n");
    printf("  3. Use valgrind to detect: Invalid free()\n");

    return 0;
}
