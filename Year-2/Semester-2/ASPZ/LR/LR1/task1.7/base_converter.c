// Завдання 1.7: Перетворення системи числення
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

int char_to_digit(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'Z') return c - 'A' + 10;
    if (c >= 'a' && c <= 'z') return c - 'a' + 10;
    return -1;
}

long convert_integer(const char *str, int base) {
    long result = 0;
    int negative = 0;
    int i = 0;

    if (str[0] == '-') {
        negative = 1;
        i = 1;
    }

    for (; str[i] != '\0'; i++) {
        int digit = char_to_digit(str[i]);
        if (digit < 0 || digit >= base) {
            continue;
        }
        result = result * base + digit;
    }

    return negative ? -result : result;
}

double convert_float_decimal(const char *str) {
    return strtod(str, NULL);
}

int main() {
    int base;
    char number[256];

    printf("Enter base (2-36): ");
    if (scanf("%d", &base) != 1 || base < 2 || base > 36) {
        fprintf(stderr, "Invalid base! Must be 2-36.\n");
        return 1;
    }

    printf("Enter number in base %d: ", base);
    if (scanf("%255s", number) != 1) {
        fprintf(stderr, "Invalid input!\n");
        return 1;
    }

    if (base == 10 && strchr(number, '.') != NULL) {
        double result = convert_float_decimal(number);
        printf("%.6f (decimal floating point)\n", result);
    } else {
        long result = convert_integer(number, base);
        printf("%ld (decimal value of %s in base %d)\n", result, number, base);
    }

    printf("\n--- Examples ---\n");
    printf("77 in base 8 = %ld\n", convert_integer("77", 8));
    printf("FF in base 16 = %ld\n", convert_integer("FF", 16));
    printf("1010 in base 2 = %ld\n", convert_integer("1010", 2));
    printf("ZZ in base 36 = %ld\n", convert_integer("ZZ", 36));

    return 0;
}
