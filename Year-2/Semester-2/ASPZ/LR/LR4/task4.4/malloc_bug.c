/*
 * Task 4.4: Find bugs in the malloc/free loop code
 *
 * Original buggy code:
 *   void *ptr = NULL;
 *   while (<condition>) {
 *       if (!ptr)
 *           ptr = malloc(n);
 *       [... use ptr ...]
 *       free(ptr);
 *   }
 *
 * Bug: After free(ptr), ptr is NOT set to NULL.
 * On next iteration, !ptr is false (ptr is dangling, not NULL),
 * so malloc is NOT called. Then ptr is used => use-after-free!
 *
 * Compile: gcc -Wall -Wextra -std=c11 -g malloc_bug.c -o malloc_bug
 * Run:     ./malloc_bug
 * Valgrind: valgrind --leak-check=full ./malloc_bug
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 64

/* Buggy version - demonstrates the problem */
static void buggy_version(void) {
    printf("--- Buggy Version ---\n");
    void *ptr = NULL;
    int iterations = 3;

    for (int i = 0; i < iterations; i++) {
        printf("  Iteration %d: ptr = %p\n", i, ptr);

        if (!ptr) {
            ptr = malloc(N);
            printf("  malloc'd: ptr = %p\n", ptr);
        } else {
            printf("  Skipped malloc (ptr not NULL, but DANGLING!)\n");
        }

        if (ptr) {
            /* Use ptr */
            memset(ptr, 'A' + i, N);
            printf("  Used ptr: first byte = '%c'\n", ((char *)ptr)[0]);
        }

        free(ptr);
        printf("  free'd ptr (but ptr still = %p, DANGLING!)\n", ptr);
        /* BUG: ptr is now dangling, NOT NULL */
        /* Next iteration: !ptr is false, malloc won't be called */
        /* Using ptr again => USE-AFTER-FREE */
    }
    printf("\n");
}

/* Fixed version 1: Set ptr to NULL after free */
static void fixed_version_1(void) {
    printf("--- Fixed Version 1: ptr = NULL after free ---\n");
    void *ptr = NULL;
    int iterations = 3;

    for (int i = 0; i < iterations; i++) {
        printf("  Iteration %d: ptr = %p\n", i, ptr);

        if (!ptr) {
            ptr = malloc(N);
            printf("  malloc'd: ptr = %p\n", ptr);
        }

        if (ptr) {
            memset(ptr, 'A' + i, N);
            printf("  Used ptr: first byte = '%c'\n", ((char *)ptr)[0]);
        }

        free(ptr);
        ptr = NULL; /* FIX: Reset to NULL */
        printf("  free'd and set to NULL\n");
    }
    printf("\n");
}

/* Fixed version 2: Allocate once, free once */
static void fixed_version_2(void) {
    printf("--- Fixed Version 2: Allocate once, free once ---\n");
    void *ptr = malloc(N);
    if (!ptr) {
        perror("malloc");
        return;
    }
    printf("  malloc'd: ptr = %p\n", ptr);

    int iterations = 3;
    for (int i = 0; i < iterations; i++) {
        printf("  Iteration %d: ", i);
        memset(ptr, 'A' + i, N);
        printf("first byte = '%c'\n", ((char *)ptr)[0]);
    }

    free(ptr);
    printf("  free'd once at the end\n\n");
}

/* Fixed version 3: Allocate/free each iteration */
static void fixed_version_3(void) {
    printf("--- Fixed Version 3: malloc/free each iteration ---\n");
    int iterations = 3;

    for (int i = 0; i < iterations; i++) {
        void *ptr = malloc(N);
        if (!ptr) {
            perror("malloc");
            continue;
        }
        printf("  Iteration %d: malloc'd %p, ", i, ptr);
        memset(ptr, 'A' + i, N);
        printf("first byte = '%c'\n", ((char *)ptr)[0]);
        free(ptr);
    }
    printf("\n");
}

int main(void) {
    printf("=== Task 4.4: malloc/free loop bug ===\n\n");

    printf("Bug explanation:\n");
    printf("  After free(ptr), ptr still holds the old address.\n");
    printf("  It is NOT automatically set to NULL.\n");
    printf("  On next iteration, (!ptr) is false, so malloc is skipped.\n");
    printf("  Using ptr after free is USE-AFTER-FREE (undefined behavior).\n\n");

    buggy_version();
    fixed_version_1();
    fixed_version_2();
    fixed_version_3();

    printf("=== Summary ===\n");
    printf("The bug: free(ptr) does not set ptr to NULL.\n");
    printf("Fix options:\n");
    printf("  1. Always set ptr = NULL after free(ptr)\n");
    printf("  2. Allocate once before loop, free after loop\n");
    printf("  3. Allocate and free within each iteration scope\n");
    printf("\nUse valgrind to detect: valgrind ./malloc_bug\n");

    return 0;
}
