#include <stdio.h>

char *mystrncpy(char *s1, const char *s2, int n) {
    char *start = s1;
    while (n-- && (*s1++ = *s2++));
    while (n-- > 0) *s1++ = '\0';
    return start;
}

int main() {
    char s1[20];
    char s2[] = "Hello";
    mystrncpy(s1, s2, 10);
    printf("Result: %s\n", s1);
    return 0;
}
