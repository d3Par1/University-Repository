/*
 * Task 5.6: Dangling pointer
 *
 * After free(p), the pointer p still holds the old address.
 * Accessing it is undefined behavior — may work, may crash,
 * or may silently corrupt other data.
 *
 * Compile: gcc -Wall -Wextra -std=c11 -g dangling_pointer.c -o dangling_pointer
 * Run:     ./dangling_pointer
 * Valgrind: valgrind ./dangling_pointer
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Basic dangling pointer */
static void demo_basic_dangling(void) {
    printf("--- Basic dangling pointer ---\n");

    int *p = malloc(sizeof(int));
    *p = 42;
    printf("  Before free: *p = %d (addr = %p)\n", *p, (void *)p);

    free(p);
    /* p is now dangling — it still holds the address but memory is freed */
    printf("  After free: *p = %d (may show old value — UB!)\n", *p);
    printf("  p = %p (still holds the address, NOT NULL)\n\n", (void *)p);
}

/* Dangling pointer hidden by new allocation reusing the address */
static void demo_reuse_address(void) {
    printf("--- Address reuse after free ---\n");

    int *p1 = malloc(sizeof(int));
    *p1 = 100;
    printf("  p1 = %p, *p1 = %d\n", (void *)p1, *p1);

    free(p1);
    /* Allocator may reuse the same address */
    int *p2 = malloc(sizeof(int));
    *p2 = 200;
    printf("  p2 = %p, *p2 = %d\n", (void *)p2, *p2);

    if (p1 == p2)
        printf("  p1 == p2! Same address reused by allocator.\n");
    else
        printf("  p1 != p2, different address assigned.\n");

    printf("  *p1 = %d (reading through dangling pointer sees p2's data!)\n",
           *p1);

    free(p2);
    printf("\n");
}

/* Safe pattern: set pointer to NULL after free */
static void demo_safe_pattern(void) {
    printf("--- Safe pattern: NULL after free ---\n");

    int *p = malloc(sizeof(int));
    *p = 42;
    printf("  *p = %d\n", *p);

    free(p);
    p = NULL; /* Prevents dangling pointer */

    if (p == NULL)
        printf("  p is NULL — safe check prevents use-after-free\n");

    printf("\n");
}

int main(void) {
    printf("=== Task 5.6: Dangling Pointer ===\n\n");

    demo_basic_dangling();
    demo_reuse_address();
    demo_safe_pattern();

    printf("=== Summary ===\n");
    printf("After free(p), ALWAYS set p = NULL.\n");
    printf("Valgrind detects access to freed memory.\n");

    return 0;
}
