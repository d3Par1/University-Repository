/*
 * Task 1.9 - String replace: spaces to given character
 *
 * Compile: gcc -Wall -o task1_9 string_replace.c
 */
#include <stdio.h>
#include <string.h>

int replace(char *str, char replacement) {
    int count = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == ' ') {
            str[i] = replacement;
            count++;
        }
    }
    return count;
}

int main(int argc, char *argv[]) {
    char str[256];
    char repl_char = '-'; /* default replacement */

    printf("=== Task 1.9: String Replace ===\n\n");

    /* Allow custom replacement character via command line */
    if (argc > 1 && strlen(argv[1]) == 1) {
        repl_char = argv[1][0];
    }

    printf("Enter a string: ");
    if (fgets(str, sizeof(str), stdin) == NULL) {
        fprintf(stderr, "Error reading input\n");
        return 1;
    }
    /* Remove newline */
    str[strcspn(str, "\n")] = '\0';

    printf("Original: \"%s\"\n", str);
    printf("Replacement char: '%c'\n", repl_char);

    int count = replace(str, repl_char);

    printf("Result:   \"%s\"\n", str);
    printf("Replacements made: %d\n", count);

    /* Demo examples */
    printf("\n--- Examples ---\n");
    char ex1[] = "The cat sat";
    int c1 = replace(ex1, '-');
    printf("\"%s\", Count: %d\n", ex1, c1);

    char ex2[] = "Hello World Program";
    int c2 = replace(ex2, '_');
    printf("\"%s\", Count: %d\n", ex2, c2);

    return 0;
}
