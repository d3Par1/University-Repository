/*
 * Task 5.9: Use-after-free
 *
 * Accessing memory after it has been freed is undefined behavior.
 * The allocator may have reused the block for a different allocation,
 * so reading returns wrong data, and writing corrupts another object.
 *
 * Compile: gcc -Wall -Wextra -std=c11 -g use_after_free.c -o use_after_free
 * Run:     ./use_after_free
 * Valgrind: valgrind ./use_after_free
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Basic use-after-free */
static void demo_basic_uaf(void) {
    printf("--- Basic use-after-free ---\n");

    char *p = malloc(16);
    strcpy(p, "Hello");
    printf("  Before free: p = \"%s\"\n", p);

    free(p);

    /* Reading freed memory — may still show old data */
    printf("  After free: p = \"%s\" (UB! Old data may linger)\n", p);

    /* Writing to freed memory — corrupts allocator state */
    p[0] = 'X';
    printf("  After write: p = \"%s\" (UB! Valgrind: Invalid write)\n", p);
    printf("\n");
}

/* Use-after-free via another allocation reusing the block */
static void demo_uaf_reuse(void) {
    printf("--- Use-after-free with block reuse ---\n");

    /* Allocate and store user data */
    char *user_data = malloc(32);
    strcpy(user_data, "user:admin");
    printf("  user_data = \"%s\"\n", user_data);

    free(user_data);
    /* user_data is now dangling */

    /* New allocation may reuse the same block */
    char *attacker_data = malloc(32);
    strcpy(attacker_data, "user:root");
    printf("  attacker_data = \"%s\" (new allocation)\n", attacker_data);

    /* Reading through old pointer sees new data */
    printf("  user_data = \"%s\" (reads attacker's data!)\n", user_data);
    printf("  This is a real-world security vulnerability.\n\n");

    free(attacker_data);
}

/* Safe pattern: nullify pointer after free */
static void demo_safe_pattern(void) {
    printf("--- Safe pattern ---\n");

    char *p = malloc(16);
    strcpy(p, "Safe");
    printf("  p = \"%s\"\n", p);

    free(p);
    p = NULL;

    /* Check before use */
    if (p != NULL)
        printf("  p = \"%s\"\n", p);
    else
        printf("  p is NULL — safe, no use-after-free\n");
    printf("\n");
}

int main(void) {
    printf("=== Task 5.9: Use-After-Free ===\n\n");

    demo_basic_uaf();
    demo_uaf_reuse();
    demo_safe_pattern();

    printf("=== Summary ===\n");
    printf("Use-after-free is one of the most exploited memory bugs.\n");
    printf("It enables privilege escalation and code execution.\n");
    printf("Valgrind reports: 'Invalid read/write ... inside a block free'd'\n");

    return 0;
}
