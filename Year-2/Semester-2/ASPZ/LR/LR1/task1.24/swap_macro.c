// Завдання 1.24: Макрос обміну значень
#include <stdio.h>

#define swap(t, x, y) do { \
    t _swap_tmp = (x);     \
    (x) = (y);             \
    (y) = _swap_tmp;       \
} while(0)

#define type_name(x) _Generic((x), \
    int: "int",                    \
    float: "float",                \
    double: "double",              \
    char: "char",                  \
    default: "unknown")

#define safe_swap(x, y) do {                               \
    _Static_assert(sizeof(x) == sizeof(y),                 \
                   "swap: types must have same size");      \
    __typeof__(x) _swap_tmp = (x);                         \
    (x) = (y);                                             \
    (y) = _swap_tmp;                                       \
} while(0)

int main() {
    printf("=== Task 1.24: Swap Macro ===\n\n");

    int a = 10, b = 20;
    printf("Before: a=%d, b=%d\n", a, b);
    swap(int, a, b);
    printf("After:  a=%d, b=%d\n\n", a, b);

    double x = 3.14, y = 2.71;
    printf("Before: x=%.2f, y=%.2f\n", x, y);
    swap(double, x, y);
    printf("After:  x=%.2f, y=%.2f\n\n", x, y);

    char c1 = 'A', c2 = 'Z';
    printf("Before: c1='%c', c2='%c'\n", c1, c2);
    swap(char, c1, c2);
    printf("After:  c1='%c', c2='%c'\n\n", c1, c2);

    int p = 100, q = 200;
    printf("safe_swap: Before p=%d, q=%d\n", p, q);
    safe_swap(p, q);
    printf("safe_swap: After  p=%d, q=%d\n", p, q);

    return 0;
}
