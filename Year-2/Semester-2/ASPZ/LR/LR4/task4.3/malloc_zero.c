/*
 * Task 4.3: What happens with malloc(0)?
 *
 * C standard says: malloc(0) returns either NULL or a unique pointer
 * that can be passed to free(). Implementation-defined behavior.
 *
 * Compile: gcc -Wall -Wextra -std=c11 malloc_zero.c -o malloc_zero
 * Run:     ltrace ./malloc_zero
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main(void) {
    printf("=== Task 4.3: malloc(0) behavior ===\n\n");

    /* Test 1: Basic malloc(0) */
    printf("--- Test 1: malloc(0) ---\n");
    errno = 0;
    void *p1 = malloc(0);
    printf("  malloc(0) = %p\n", p1);
    if (p1)
        printf("  Returns non-NULL pointer (implementation: glibc-style)\n");
    else
        printf("  Returns NULL (implementation: minimal)\n");
    printf("  errno = %d\n", errno);

    /* Test 2: Multiple malloc(0) - are pointers unique? */
    printf("\n--- Test 2: Multiple malloc(0) calls ---\n");
    void *ptrs[5];
    for (int i = 0; i < 5; i++) {
        ptrs[i] = malloc(0);
        printf("  malloc(0)[%d] = %p\n", i, ptrs[i]);
    }

    /* Check uniqueness */
    int all_unique = 1;
    for (int i = 0; i < 5; i++)
        for (int j = i + 1; j < 5; j++)
            if (ptrs[i] == ptrs[j] && ptrs[i] != NULL)
                all_unique = 0;
    printf("  All pointers unique: %s\n", all_unique ? "yes" : "no");

    /* Free all */
    for (int i = 0; i < 5; i++)
        free(ptrs[i]);
    printf("  All freed successfully\n");

    /* Test 3: free(malloc(0)) */
    printf("\n--- Test 3: free(malloc(0)) ---\n");
    void *p3 = malloc(0);
    printf("  p = malloc(0) = %p\n", p3);
    free(p3);
    printf("  free(p) succeeded (no crash)\n");

    /* Test 4: Can we write to malloc(0) result? (UB but interesting) */
    printf("\n--- Test 4: Using malloc(0) pointer ---\n");
    void *p4 = malloc(0);
    printf("  p = malloc(0) = %p\n", p4);
    if (p4) {
        printf("  Writing to p[0] is UNDEFINED BEHAVIOR!\n");
        printf("  (The pointer is valid but has 0 usable size)\n");
        printf("  In practice, glibc allocates minimum chunk (typically 16-32 bytes)\n");
        printf("  But relying on this is a bug.\n");
    }
    free(p4);

    /* Test 5: calloc(0, ...) and calloc(..., 0) */
    printf("\n--- Test 5: calloc with zero ---\n");
    void *c1 = calloc(0, sizeof(int));
    void *c2 = calloc(10, 0);
    void *c3 = calloc(0, 0);
    printf("  calloc(0, sizeof(int)) = %p\n", c1);
    printf("  calloc(10, 0)          = %p\n", c2);
    printf("  calloc(0, 0)           = %p\n", c3);
    free(c1);
    free(c2);
    free(c3);
    printf("  All freed successfully\n");

    /* Test 6: realloc to 0 */
    printf("\n--- Test 6: realloc(p, 0) ---\n");
    void *p6 = malloc(100);
    printf("  p = malloc(100) = %p\n", p6);
    void *p6r = realloc(p6, 0);
    printf("  realloc(p, 0) = %p\n", p6r);
    printf("  (In glibc, this may free p and return NULL or small ptr)\n");
    free(p6r); /* Safe even if NULL */

    printf("\n=== Explanation ===\n");
    printf("C11 standard (7.22.3):\n");
    printf("  'If size is zero, the behavior is implementation-defined:\n");
    printf("   either a null pointer is returned, or the behavior is as\n");
    printf("   if size were some nonzero value, except that the returned\n");
    printf("   pointer shall not be used to access an object.'\n\n");
    printf("glibc behavior:\n");
    printf("  - malloc(0) returns a unique non-NULL pointer\n");
    printf("  - Minimum allocation is typically 16-32 bytes internally\n");
    printf("  - The pointer can be passed to free()\n");
    printf("  - Writing through the pointer is undefined behavior\n");
    printf("\nRun with ltrace to see actual malloc/free calls:\n");
    printf("  ltrace ./malloc_zero 2>&1 | head -30\n");

    return 0;
}
