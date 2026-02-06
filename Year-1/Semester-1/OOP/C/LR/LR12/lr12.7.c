#include <stdio.h>

int is_within(char c, const char *str) {
    while (*str) {
        if (*str == c) return 1;
        str++;
    }
    return 0;
}

int main() {
    char str[] = "Hello, world!";
    char ch = 'o';
    printf("Character '%c' %s in the string.\n", ch, is_within(ch, str) ? "is" : "is not");
    return 0;
}
