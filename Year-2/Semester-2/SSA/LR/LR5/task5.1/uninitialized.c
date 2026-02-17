/*
 * Task 5.1: Uninitialized variables (UV)
 *
 * Demonstrates using a variable before initialization.
 * The stack contains "garbage" from previous function calls,
 * so uninitialized local variables have unpredictable values.
 * This is not always UB, but leads to non-deterministic behavior.
 *
 * Compile: gcc -Wall -Wextra -std=c11 -g uninitialized.c -o uninitialized
 * Run:     ./uninitialized
 * Valgrind: valgrind --track-origins=yes ./uninitialized
 */

#include <stdio.h>

/* Fill stack with known pattern so next function sees "garbage" */
static void fill_stack(void) {
    volatile int arr[64];
    for (int i = 0; i < 64; i++)
        arr[i] = 0xDEADBEEF;
    (void)arr;
}

static void demo_uninitialized(void) {
    int x;      /* not initialized */
    int y;      /* not initialized */
    double z;   /* not initialized */

    printf("--- Reading uninitialized variables ---\n");
    printf("  x = %d  (expected: garbage)\n", x);
    printf("  y = %d  (expected: garbage)\n", y);
    printf("  z = %f  (expected: garbage)\n", z);
    printf("\n");

    /* Conditional on uninitialized value — unpredictable branch */
    if (x > 0)
        printf("  x > 0 — branch taken (unpredictable!)\n");
    else
        printf("  x <= 0 — branch taken (unpredictable!)\n");
}

/* Show that uninitialized array elements also contain garbage */
static void demo_uninitialized_array(void) {
    int arr[8]; /* not initialized */

    printf("--- Uninitialized array ---\n");
    for (int i = 0; i < 8; i++)
        printf("  arr[%d] = %d\n", i, arr[i]);
    printf("\n");
}

int main(void) {
    printf("=== Task 5.1: Uninitialized Variables ===\n\n");

    fill_stack();
    demo_uninitialized();
    demo_uninitialized_array();

    printf("=== Summary ===\n");
    printf("Uninitialized variables read whatever is on the stack.\n");
    printf("Use valgrind --track-origins=yes to detect such issues.\n");

    return 0;
}
