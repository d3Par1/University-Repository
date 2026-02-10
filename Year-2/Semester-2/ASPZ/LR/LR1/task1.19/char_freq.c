/*
 * Task 1.19 - Character frequency from file, sorted by descending frequency
 *
 * Compile: gcc -Wall -o task1_19 char_freq.c
 * Usage:   ./task1_19 file.txt
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct {
    int ch;
    int count;
} CharFreq;

int cmp_freq_desc(const void *a, const void *b) {
    return ((CharFreq *)b)->count - ((CharFreq *)a)->count;
}

int main(int argc, char *argv[]) {
    FILE *fp = stdin;
    if (argc > 1) {
        fp = fopen(argv[1], "r");
        if (!fp) { perror("fopen"); return 1; }
    }

    int freq[256] = {0};
    int c;
    long total = 0;

    while ((c = fgetc(fp)) != EOF) {
        freq[c]++;
        total++;
    }
    if (fp != stdin) fclose(fp);

    /* Build sorted array */
    CharFreq arr[256];
    int n = 0;
    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            arr[n].ch = i;
            arr[n].count = freq[i];
            n++;
        }
    }

    qsort(arr, n, sizeof(CharFreq), cmp_freq_desc);

    printf("=== Character Frequency (total %ld chars) ===\n\n", total);
    printf("%-10s %-8s %-8s\n", "Char", "Count", "Percent");
    printf("------------------------------\n");
    for (int i = 0; i < n; i++) {
        char display[16];
        if (arr[i].ch == '\n') snprintf(display, 16, "\\n");
        else if (arr[i].ch == '\t') snprintf(display, 16, "\\t");
        else if (arr[i].ch == ' ') snprintf(display, 16, "SPACE");
        else if (isprint(arr[i].ch)) snprintf(display, 16, "'%c'", arr[i].ch);
        else snprintf(display, 16, "0x%02X", arr[i].ch);

        printf("%-10s %-8d %.2f%%\n", display, arr[i].count,
               100.0 * arr[i].count / total);
    }

    return 0;
}
