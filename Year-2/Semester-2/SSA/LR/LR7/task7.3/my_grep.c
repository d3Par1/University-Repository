#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Task 7.3: Minimal `grep` — prints lines containing the word given as
 * argv[1] from the file given as argv[2] (or stdin if absent).
 * Match is case-sensitive substring (strstr), not a regex.
 */
int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "usage: %s WORD [FILE]\n", argv[0]);
        return 2;
    }

    FILE *in = stdin;
    if (argc >= 3) {
        in = fopen(argv[2], "r");
        if (!in) { perror(argv[2]); return 1; }
    }

    const char *needle = argv[1];
    char *line = NULL;
    size_t cap = 0;
    long lineno = 0;
    while (getline(&line, &cap, in) != -1) {
        ++lineno;
        if (strstr(line, needle)) {
            printf("%ld: %s", lineno, line);
        }
    }
    free(line);
    if (in != stdin) fclose(in);
    return 0;
}
