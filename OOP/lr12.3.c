#include <stdio.h>

char *my_gets(char *str) {
    char *ptr = str;
    while ((*ptr = getchar()) != '\n' && *ptr != EOF) {
        ptr++;
    }
    *ptr = '\0';
    return str;
}

int main() {
    char input[100];
    printf("Enter a string: ");
    my_gets(input);
    printf("You entered: %s\n", input);
    return 0;
}
