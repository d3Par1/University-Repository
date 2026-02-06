#include <stdio.h>

void read_chars(char *arr, int n) {
    char c;
    int i = 0;
    while (i < n && (c = getchar()) != EOF) {
        if (c == ' ' || c == '\t' || c == '\n') break;
        arr[i++] = c;
    }
    arr[i] = '\0';
}

int main() {
    char input[100];
    printf("Enter characters: ");
    read_chars(input, 10);
    printf("You entered: %s\n", input);
    return 0;
}
