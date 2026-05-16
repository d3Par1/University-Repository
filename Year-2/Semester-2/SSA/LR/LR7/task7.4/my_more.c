#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PAGE 20

/*
 * Task 7.4: Minimal `more` — prints each file given as arg, pausing every
 * 20 lines for a keypress on /dev/tty (so it works even if stdin is piped).
 */
static int prompt_continue(void)
{
    FILE *tty = fopen("/dev/tty", "r");
    if (!tty) return 0;
    fprintf(stderr, "--More-- press ENTER or 'q' to quit ");
    int c = fgetc(tty);
    fclose(tty);
    return c == 'q' || c == 'Q' || c == EOF;
}

static int dump_file(const char *name)
{
    FILE *f = fopen(name, "r");
    if (!f) { perror(name); return -1; }

    char *line = NULL;
    size_t cap = 0;
    int count = 0;
    while (getline(&line, &cap, f) != -1) {
        fputs(line, stdout);
        if (++count % PAGE == 0) {
            fflush(stdout);
            if (prompt_continue()) { free(line); fclose(f); return 1; }
        }
    }
    free(line);
    fclose(f);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "usage: %s FILE [FILE...]\n", argv[0]);
        return 2;
    }
    for (int i = 1; i < argc; ++i) {
        int r = dump_file(argv[i]);
        if (r == 1) break;     /* user quit */
    }
    return 0;
}
