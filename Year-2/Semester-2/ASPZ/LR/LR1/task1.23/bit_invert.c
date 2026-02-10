/*
 * Task 1.23 - Bit inversion (full and range-based)
 *
 * Compile: gcc -Wall -o task1_23 bit_invert.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void print_binary_8(uint8_t val) {
    for (int i = 7; i >= 0; i--) printf("%d", (val >> i) & 1);
}

/* Full inversion */
uint8_t invert_all(uint8_t x) {
    return ~x;
}

/* Invert bits in range [from, to] (0-indexed from LSB) */
uint8_t invert_range(uint8_t x, int from, int to) {
    if (from > to) { int t = from; from = to; to = t; }
    if (to > 7) to = 7;
    if (from < 0) from = 0;

    uint8_t mask = 0;
    for (int i = from; i <= to; i++) {
        mask |= (1 << i);
    }
    return x ^ mask;
}

int main(int argc, char *argv[]) {
    printf("=== Task 1.23: Bit Inversion ===\n\n");

    uint8_t x = 0xAA; /* 10101010 */
    if (argc > 1) x = (uint8_t)strtoul(argv[1], NULL, 0);

    uint8_t y = invert_all(x);

    printf("x        = "); print_binary_8(x); printf(" (decimal %u)\n", x);
    printf("inverted = "); print_binary_8(y); printf(" (decimal %u)\n", y);

    /* Range inversion */
    printf("\n--- Range Inversion ---\n");
    int from = 2, to = 5;
    if (argc > 3) { from = atoi(argv[2]); to = atoi(argv[3]); }

    uint8_t z = invert_range(x, from, to);
    printf("x              = "); print_binary_8(x); printf("\n");
    printf("invert[%d..%d]  = ", from, to); print_binary_8(z);
    printf(" (decimal %u)\n", z);

    /* More examples */
    printf("\n--- Examples ---\n");
    uint8_t tests[] = {0x00, 0xFF, 0x0F, 0xF0, 0x55};
    for (int i = 0; i < 5; i++) {
        printf("  "); print_binary_8(tests[i]);
        printf(" -> "); print_binary_8(invert_all(tests[i]));
        printf("  (%3u -> %3u)\n", tests[i], invert_all(tests[i]));
    }

    return 0;
}
