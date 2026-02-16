/*
 * Task 5.4: Undefined behavior (UB)
 *
 * Demonstrates several forms of undefined behavior in C.
 * The compiler is free to produce any result, including "working" code.
 * Different optimization levels or compilers may give different results.
 *
 * Compile: gcc -Wall -Wextra -std=c11 -g undefined_behavior.c -o undefined_behavior
 * Run:     ./undefined_behavior
 * Compare: gcc -O0 vs gcc -O2 may produce different outputs
 */

#include <stdio.h>
#include <limits.h>

/* UB: Sequence point violation */
static void demo_sequence_point(void) {
    int a = 10;

    printf("--- Sequence point violation ---\n");
    printf("  a = %d (initial)\n", a);

    /* a++ + ++a: order of evaluation is not defined by the standard */
    int b = a++ + ++a;
    printf("  b = a++ + ++a = %d (result is compiler-dependent!)\n", b);
    printf("  a = %d after expression\n", a);
    printf("\n");
}

/* UB: Signed integer overflow */
static void demo_signed_overflow(void) {
    printf("--- Signed integer overflow ---\n");
    int x = INT_MAX;
    printf("  INT_MAX = %d\n", x);

    /* Adding 1 to INT_MAX is undefined behavior for signed int */
    int y = x + 1;
    printf("  INT_MAX + 1 = %d (UB! Compiler may optimize this away)\n", y);
    printf("\n");
}

/* UB: Division by zero */
static void demo_division_by_zero(void) {
    printf("--- Division by zero (skipped to avoid crash) ---\n");
    printf("  int x = 1 / 0; would cause UB (SIGFPE or worse)\n");
    printf("  Floating-point: 1.0/0.0 = inf (defined by IEEE 754)\n");
    double d = 1.0 / 0.0;
    printf("  1.0 / 0.0 = %f\n", d);
    printf("\n");
}

/* UB: Null pointer dereference (shown but not executed) */
static void demo_null_deref_info(void) {
    printf("--- Null pointer dereference ---\n");
    printf("  int *p = NULL; *p = 42; would cause SIGSEGV\n");
    printf("  Compiler may optimize assuming p != NULL\n");
    printf("\n");
}

/* UB: Shift by negative or too-large amount */
static void demo_shift_ub(void) {
    printf("--- Shift undefined behavior ---\n");
    int x = 1;

    /* Shift by >= bit width is UB */
    int y = x << 31;
    printf("  1 << 31 = %d (may be UB for signed int)\n", y);

    /* Shift negative value is UB in C99/C11 */
    int z = -1;
    int w = z << 1;
    printf("  -1 << 1 = %d (UB: left-shifting negative value)\n", w);
    printf("\n");
}

int main(void) {
    printf("=== Task 5.4: Undefined Behavior ===\n\n");

    demo_sequence_point();
    demo_signed_overflow();
    demo_division_by_zero();
    demo_null_deref_info();
    demo_shift_ub();

    printf("=== Summary ===\n");
    printf("UB means the compiler can do ANYTHING.\n");
    printf("Code that 'works' today may break with -O2 or a new compiler.\n");
    printf("Use -fsanitize=undefined to detect UB at runtime.\n");

    return 0;
}
