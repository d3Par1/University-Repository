/*
 * Task 1.24 - swap macro with type safety check
 *
 * Compile: gcc -Wall -o task1_24 swap_macro.c
 */
#include <stdio.h>

/* Basic swap macro */
#define swap(t, x, y) do { \
    t _swap_tmp = (x);     \
    (x) = (y);             \
    (y) = _swap_tmp;       \
} while(0)

/* Type-checking version using _Generic (C11) */
#define type_name(x) _Generic((x), \
    int: "int",                    \
    float: "float",                \
    double: "double",              \
    char: "char",                  \
    default: "unknown")

/* Swap with type compatibility check (compile-time via _Generic)
 * Note: typeof() is a GCC/Clang extension (standard in C23) */
#define safe_swap(x, y) do {                               \
    _Static_assert(sizeof(x) == sizeof(y),                 \
                   "swap: types must have same size");      \
    __typeof__(x) _swap_tmp = (x);                         \
    (x) = (y);                                             \
    (y) = _swap_tmp;                                       \
} while(0)

int main() {
    printf("=== Task 1.24: Swap Macro ===\n\n");

    /* int swap */
    int a = 10, b = 20;
    printf("Before: a=%d, b=%d\n", a, b);
    swap(int, a, b);
    printf("After:  a=%d, b=%d\n\n", a, b);

    /* double swap */
    double x = 3.14, y = 2.71;
    printf("Before: x=%.2f, y=%.2f\n", x, y);
    swap(double, x, y);
    printf("After:  x=%.2f, y=%.2f\n\n", x, y);

    /* char swap */
    char c1 = 'A', c2 = 'Z';
    printf("Before: c1='%c', c2='%c'\n", c1, c2);
    swap(char, c1, c2);
    printf("After:  c1='%c', c2='%c'\n\n", c1, c2);

    /* safe_swap (C11) */
    int p = 100, q = 200;
    printf("safe_swap: Before p=%d, q=%d\n", p, q);
    safe_swap(p, q);
    printf("safe_swap: After  p=%d, q=%d\n", p, q);

    /* This would cause compile error: */
    /* int i = 5; double d = 3.0; safe_swap(i, d); */

    return 0;
}
