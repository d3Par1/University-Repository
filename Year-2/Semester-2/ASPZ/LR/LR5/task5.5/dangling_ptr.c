/*
 * Task 5.5: Dangling pointer demonstration
 *
 * A dangling pointer points to memory that has been freed.
 * Reading it may return the old value (by luck), but writing
 * through it corrupts the heap.
 *
 * Compile: gcc -Wall -Wextra -std=c11 -g dangling_ptr.c -o dangling_ptr
 * Run:     ./dangling_ptr
 * Valgrind: valgrind ./dangling_ptr
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Demo 1: Read after free */
static void demo_read_after_free(void) {
    printf("--- Demo 1: Read after free ---\n");
    int *p = malloc(sizeof(int));
    if (!p) return;
    *p = 42;
    printf("  Before free: *p = %d, p = %p\n", *p, (void *)p);

    free(p);
    /* p is now dangling — it still holds the old address */
    printf("  After free: p = %p (dangling!)\n", (void *)p);
    printf("  *p may still show 42 by luck, but this is UB.\n");
    printf("  Valgrind: Invalid read of size 4\n\n");
}

/* Demo 2: Write after free corrupts heap */
static void demo_write_after_free(void) {
    printf("--- Demo 2: Write after free ---\n");
    char *buf = malloc(32);
    if (!buf) return;
    strcpy(buf, "hello");
    printf("  Before free: buf = \"%s\"\n", buf);

    free(buf);
    /* Writing to freed memory — corrupts heap metadata */
    printf("  After free: writing to freed memory...\n");
    printf("  buf[0] = 'X' would corrupt heap (skipped for safety)\n");
    printf("  Valgrind: Invalid write of size 1\n\n");
}

/* Demo 3: Returning pointer to local variable */
static int *get_local_ptr(void) {
    int local_val = 100;
    return &local_val; /* WARNING: returning address of local variable */
}

static void demo_stack_dangling(void) {
    printf("--- Demo 3: Pointer to local variable ---\n");
    int *p = get_local_ptr();
    printf("  p points to destroyed stack frame\n");
    printf("  *p might work by luck, but the value is unreliable\n");
    printf("  Compiler warns: 'address of local variable returned'\n\n");
}

/* Demo 4: Safe pattern — set to NULL after free */
static void demo_safe_pattern(void) {
    printf("--- Demo 4: Safe pattern ---\n");
    int *p = malloc(sizeof(int));
    if (!p) return;
    *p = 42;
    printf("  *p = %d\n", *p);

    free(p);
    p = NULL; /* Safe: now we can check if (p) before use */
    printf("  After free: p = %p (set to NULL)\n", (void *)p);

    if (p) {
        printf("  Using p...\n");
    } else {
        printf("  p is NULL — safely avoided dangling access\n\n");
    }
}

int main(void) {
    printf("=== Task 5.5: Dangling Pointers ===\n\n");

    printf("A dangling pointer points to freed or out-of-scope memory.\n");
    printf("Accessing it is undefined behavior — may work, crash,\n");
    printf("or silently corrupt data.\n\n");

    demo_read_after_free();
    demo_write_after_free();
    demo_stack_dangling();
    demo_safe_pattern();

    printf("=== Summary ===\n");
    printf("Prevention: always set pointer to NULL after free().\n");
    printf("Detection: valgrind, AddressSanitizer (-fsanitize=address).\n");

    return 0;
}
