/*
 * Task 4.6: realloc with NULL pointer or size 0
 *
 * C standard behavior:
 *   realloc(NULL, size) == malloc(size)
 *   realloc(ptr, 0)     == free(ptr), returns NULL (or implementation-defined)
 *
 * Compile: gcc -Wall -Wextra -std=c11 realloc_null_zero.c -o realloc_null_zero
 * Run:     ltrace ./realloc_null_zero
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    printf("=== Task 4.6: realloc with NULL and 0 ===\n\n");

    /* Test 1: realloc(NULL, size) - acts like malloc(size) */
    printf("--- Test 1: realloc(NULL, size) ---\n");
    printf("  C standard: realloc(NULL, size) is equivalent to malloc(size)\n\n");

    void *p1 = realloc(NULL, 100);
    printf("  realloc(NULL, 100) = %p\n", p1);
    if (p1) {
        strcpy(p1, "Allocated via realloc(NULL, ...)");
        printf("  Content: \"%s\"\n", (char *)p1);
    }

    /* Can keep growing with realloc */
    void *p2 = realloc(p1, 200);
    printf("  realloc(%p, 200) = %p\n", p1, p2);
    if (p2) {
        printf("  Content preserved: \"%s\"\n", (char *)p2);
        p1 = p2; /* Update pointer */
    }
    free(p1);

    /* Test 2: realloc(ptr, 0) - may act like free(ptr) */
    printf("\n--- Test 2: realloc(ptr, 0) ---\n");
    printf("  C11: 'If size is 0, implementation-defined'\n");
    printf("  C17/C23: deprecated, new standard says it's UB if size=0\n");
    printf("  glibc: traditionally frees and returns NULL\n\n");

    void *p3 = malloc(100);
    printf("  p = malloc(100) = %p\n", p3);
    if (p3)
        strcpy(p3, "Will be freed");

    void *p4 = realloc(p3, 0);
    printf("  realloc(p, 0) = %p\n", p4);
    if (p4 == NULL)
        printf("  Returned NULL (freed the memory, glibc-style)\n");
    else {
        printf("  Returned non-NULL (kept minimum allocation)\n");
        free(p4);
    }
    /* Don't free p3 - it was freed by realloc */

    /* Test 3: realloc(NULL, 0) */
    printf("\n--- Test 3: realloc(NULL, 0) ---\n");
    void *p5 = realloc(NULL, 0);
    printf("  realloc(NULL, 0) = %p\n", p5);
    if (p5)
        printf("  Non-NULL: acts like malloc(0)\n");
    else
        printf("  NULL: no allocation made\n");
    free(p5); /* Safe even if NULL */

    /* Test 4: Using realloc(NULL, ...) to build a dynamic array */
    printf("\n--- Test 4: Building array with realloc(NULL, ...) ---\n");
    int *arr = NULL;
    size_t capacity = 0;

    for (int i = 0; i < 10; i++) {
        int *tmp = realloc(arr, (capacity + 1) * sizeof(int));
        if (!tmp) {
            perror("realloc");
            break;
        }
        arr = tmp;
        arr[capacity] = i * i;
        capacity++;
    }

    printf("  Array (%zu elements): ", capacity);
    for (size_t i = 0; i < capacity; i++)
        printf("%d ", arr[i]);
    printf("\n");
    free(arr);

    /* Test 5: Shrinking with realloc */
    printf("\n--- Test 5: Shrinking with realloc ---\n");
    char *str = malloc(100);
    if (str) {
        strcpy(str, "Hello, World! This is a long string.");
        printf("  Before: \"%s\" (allocated 100)\n", str);

        char *shrunk = realloc(str, 14);
        if (shrunk) {
            shrunk[13] = '\0';
            printf("  After realloc(14): \"%s\"\n", shrunk);
            free(shrunk);
        } else {
            free(str);
        }
    }

    printf("\n=== Summary ===\n");
    printf("realloc(NULL, n):  equivalent to malloc(n)\n");
    printf("realloc(ptr, 0):   implementation-defined (may free ptr)\n");
    printf("realloc(NULL, 0):  equivalent to malloc(0)\n");
    printf("\nNote: realloc(ptr, 0) behavior varies:\n");
    printf("  - glibc: frees and returns NULL\n");
    printf("  - musl: returns unique pointer (like malloc(0))\n");
    printf("  - C23: makes this undefined behavior\n");
    printf("  - Best practice: use free() explicitly to free memory\n");

    return 0;
}
