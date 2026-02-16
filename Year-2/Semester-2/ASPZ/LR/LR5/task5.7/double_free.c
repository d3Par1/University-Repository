/*
 * Task 5.7: Double free
 *
 * Calling free() twice on the same pointer corrupts the heap allocator's
 * internal data structures. This can cause crashes, heap poisoning,
 * or security vulnerabilities.
 *
 * Compile: gcc -Wall -Wextra -std=c11 -g double_free.c -o double_free
 * Run:     ./double_free
 * Valgrind: valgrind ./double_free
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Demonstrate double free detection */
static void demo_double_free(void) {
    printf("--- Double free ---\n");

    int *p = malloc(sizeof(int));
    *p = 42;
    printf("  Allocated: *p = %d at %p\n", *p, (void *)p);

    free(p);
    printf("  First free() — OK\n");

    /*
     * free(p) again would be double-free.
     * Modern glibc detects this: "double free or corruption (fasttop)"
     * We skip actual execution to avoid abort().
     */
    printf("  Second free(p) would cause:\n");
    printf("    - glibc: 'double free or corruption' -> abort()\n");
    printf("    - Valgrind: 'Invalid free()'\n");
    printf("    - Security risk: heap poisoning attack\n");
    printf("\n");
}

/* Safe pattern: NULL after free prevents double free */
static void demo_safe_double_free(void) {
    printf("--- Safe pattern: NULL guard ---\n");

    int *p = malloc(sizeof(int));
    *p = 42;
    printf("  *p = %d\n", *p);

    free(p);
    p = NULL;
    printf("  After free: p = NULL\n");

    /* free(NULL) is safe and does nothing */
    free(p);
    printf("  free(NULL) — safe, does nothing\n");
    printf("\n");
}

/* Common bug: two pointers to the same allocation */
static void demo_aliased_double_free(void) {
    printf("--- Aliased pointers — hidden double free ---\n");

    int *p = malloc(sizeof(int));
    int *q = p; /* q is an alias to p */

    *p = 99;
    printf("  p = %p, q = %p (same address)\n", (void *)p, (void *)q);

    free(p);
    printf("  free(p) — OK\n");
    printf("  free(q) would be double free (q == p)!\n");

    /* Fix: track ownership — only one pointer should own the memory */
    p = NULL;
    q = NULL;
    printf("  Set both to NULL to prevent accidental double free\n");
    printf("\n");
}

int main(void) {
    printf("=== Task 5.7: Double Free ===\n\n");

    demo_double_free();
    demo_safe_double_free();
    demo_aliased_double_free();

    printf("=== Summary ===\n");
    printf("Double free corrupts the heap and may be exploitable.\n");
    printf("Prevention: set pointer to NULL after free.\n");
    printf("free(NULL) is always safe (C standard guarantees this).\n");

    return 0;
}
