/*
 * Task 5.2: Out-of-bounds access
 *
 * Demonstrates reading and writing beyond array boundaries.
 * On most systems this does NOT crash immediately because
 * adjacent stack memory is still mapped. This silent corruption
 * is one of the most dangerous bugs in C.
 *
 * Compile: gcc -Wall -Wextra -std=c11 -g out_of_bounds.c -o out_of_bounds
 * Run:     ./out_of_bounds
 * Valgrind: valgrind ./out_of_bounds
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Stack out-of-bounds: read past array end */
static void demo_stack_oob_read(void) {
    int a[3] = {10, 20, 30};

    printf("--- Stack out-of-bounds READ ---\n");
    printf("  Array: a[0]=%d, a[1]=%d, a[2]=%d\n", a[0], a[1], a[2]);
    printf("  a[3]=%d (out-of-bounds, reads adjacent stack memory)\n", a[3]);
    printf("  a[5]=%d (further out, may read other variables)\n", a[5]);
    printf("\n");
}

/* Stack out-of-bounds: write corrupts adjacent variable */
static void demo_stack_oob_write(void) {
    int guard = 0x41414141;
    int a[3] = {1, 2, 3};

    printf("--- Stack out-of-bounds WRITE ---\n");
    printf("  Before: guard = 0x%X, a = {%d, %d, %d}\n",
           guard, a[0], a[1], a[2]);

    /* Write past array end — may overwrite guard */
    a[3] = 99;
    a[4] = 88;

    printf("  After a[3]=99, a[4]=88:\n");
    printf("  guard = 0x%X (may be corrupted!)\n", guard);
    printf("  a = {%d, %d, %d}\n", a[0], a[1], a[2]);
    printf("\n");
}

/* Heap out-of-bounds */
static void demo_heap_oob(void) {
    int *arr = malloc(3 * sizeof(int));
    if (!arr) return;

    arr[0] = 100;
    arr[1] = 200;
    arr[2] = 300;

    printf("--- Heap out-of-bounds ---\n");
    printf("  Allocated 3 ints on heap\n");
    printf("  arr[0]=%d, arr[1]=%d, arr[2]=%d\n", arr[0], arr[1], arr[2]);

    /* Read beyond allocated region */
    printf("  arr[3]=%d (out-of-bounds heap read)\n", arr[3]);
    printf("  Valgrind will report: Invalid read of size 4\n");

    free(arr);
    printf("\n");
}

int main(void) {
    printf("=== Task 5.2: Out-of-Bounds Access ===\n\n");

    demo_stack_oob_read();
    demo_stack_oob_write();
    demo_heap_oob();

    printf("=== Summary ===\n");
    printf("Out-of-bounds access silently corrupts data.\n");
    printf("No crash does NOT mean no bug. Use valgrind to detect.\n");

    return 0;
}
