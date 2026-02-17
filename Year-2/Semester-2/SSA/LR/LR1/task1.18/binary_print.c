// Завдання 1.18: Двійкове представлення числа
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

void print_binary_bitwise(uint64_t num, int bits) {
    for (int i = bits - 1; i >= 0; i--) {
        printf("%d", (int)((num >> i) & 1));
        if (i > 0 && i % 4 == 0) printf(" ");
    }
}

static const char *nibble_table[] = {
    "0000", "0001", "0010", "0011", "0100", "0101", "0110", "0111",
    "1000", "1001", "1010", "1011", "1100", "1101", "1110", "1111"
};

void print_binary_table(uint64_t num, int bits) {
    int nibbles = bits / 4;
    for (int i = nibbles - 1; i >= 0; i--) {
        int nibble = (num >> (i * 4)) & 0xF;
        printf("%s", nibble_table[nibble]);
        if (i > 0) printf(" ");
    }
}

void print_formatted(uint64_t num, int bits) {
    printf("  Bitwise: ");
    print_binary_bitwise(num, bits);
    printf("\n  Table:   ");
    print_binary_table(num, bits);
    printf("\n  Decimal: %" PRIu64 "\n  Hex:     0x%0*" PRIX64 "\n",
           num, bits / 4, num);
}

void text_to_binary(const char *text) {
    printf("Text: \"%s\"\nBinary ASCII:\n  ", text);
    for (int i = 0; text[i]; i++) {
        print_binary_bitwise((uint8_t)text[i], 8);
        printf(" ");
        if ((i + 1) % 4 == 0) printf("\n  ");
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    printf("=== Task 1.18: Binary Representation ===\n\n");

    uint8_t val8 = 0xA5;
    printf("--- 8-bit: %u (0x%02X) ---\n", val8, val8);
    print_formatted(val8, 8);

    uint16_t val16 = 0xBEEF;
    printf("\n--- 16-bit: %u (0x%04X) ---\n", val16, val16);
    print_formatted(val16, 16);

    uint32_t val32 = 0xDEADBEEF;
    printf("\n--- 32-bit: %u (0x%08X) ---\n", val32, val32);
    print_formatted(val32, 32);

    uint64_t val64 = 0x0123456789ABCDEFULL;
    printf("\n--- 64-bit: %" PRIu64 " (0x%016" PRIX64 ") ---\n",
           val64, val64);
    print_formatted(val64, 64);

    if (argc > 1) {
        unsigned long uval = strtoul(argv[1], NULL, 0);
        int bits = 8;
        if (uval > 0xFF) bits = 16;
        if (uval > 0xFFFF) bits = 32;
        if (uval > 0xFFFFFFFF) bits = 64;
        printf("\n--- User input: %lu (%d-bit) ---\n", uval, bits);
        print_formatted(uval, bits);
    }

    printf("\n--- ASCII to Binary ---\n");
    text_to_binary("Hello!");

    return 0;
}
