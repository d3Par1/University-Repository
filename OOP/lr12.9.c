#include <stdio.h>
#include <string.h>

char *string_in(const char *str1, const char *str2) {
    return strstr(str1, str2);
}

int main() {
    char str1[] = "hats";
    char str2[] = "at";
    char *result = string_in(str1, str2);
    if (result) {
        printf("Found at position: %ld\n", result - str1);
    } else {
        printf("Not found\n");
    }
    return 0;
}
