#include <stdio.h>

int my_strlen(const char *str) {
    const char *ptr = str;
    while (*ptr) {
        ptr++;
    }
    return ptr - str;
}

int main() {
    char str[] = "Hello, world!";
    printf("Length: %d\n", my_strlen(str));
    return 0;
}
