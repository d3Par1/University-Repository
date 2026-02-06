#include <stdio.h>
#include <string.h>

void reverse_print(const char *str) {
    const char *ptr = str + strlen(str) - 1;
    while (ptr >= str) {
        putchar(*ptr--);
    }
    putchar('\n');
}

int main() {
    char str[] = "Hello, world!";
    reverse_print(str);
    return 0;
}
