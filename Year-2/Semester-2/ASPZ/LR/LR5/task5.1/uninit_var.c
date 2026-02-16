/*
 * Task 5.1: Uninitialized variables (UV)
 *
 * Demonstrates using a variable before initialization.
 * On most systems this won't crash — stack memory contains "garbage"
 * from previous function calls.
 *
 * Compile: gcc -Wall -Wextra -std=c11 -g uninit_var.c -o uninit_var
 * Run:     ./uninit_var
 * Valgrind: valgrind --track-origins=yes ./uninit_var
 */

#include <stdio.h>

static void demo_uninitialized_int(void) {
    printf("--- Demo 1: Uninitialized int ---\n");
    int x;
    /* x contains whatever was on the stack before */
    printf("  x = %d (garbage value from stack)\n", x);
    printf("  &x = %p\n\n", (void *)&x);
}

static void demo_uninitialized_array(void) {
    printf("--- Demo 2: Uninitialized array ---\n");
    int arr[5];
    /* Array elements are not zeroed — they hold stack residue */
    for (int i = 0; i < 5; i++) {
        printf("  arr[%d] = %d\n", i, arr[i]);
    }
    printf("\n");
}

static void demo_conditional_init(void) {
    printf("--- Demo 3: Conditionally initialized variable ---\n");
    int y;
    int condition = 0; /* simulate rare branch */

    if (condition) {
        y = 42;
    }
    /* y is uninitialized if condition == 0 */
    printf("  condition = %d, y = %d (may be garbage)\n\n", condition, y);
}

int main(void) {
    printf("=== Task 5.1: Uninitialized Variables ===\n\n");

    printf("Stack memory is NOT zeroed in C.\n");
    printf("Reading uninitialized vars gives unpredictable values.\n");
    printf("Valgrind with --track-origins=yes detects this.\n\n");

    demo_uninitialized_int();
    demo_uninitialized_array();
    demo_conditional_init();

    printf("=== Summary ===\n");
    printf("Uninitialized variables may silently produce wrong results.\n");
    printf("Always initialize variables before use.\n");
    printf("Use: valgrind --track-origins=yes ./uninit_var\n");

    return 0;
}
