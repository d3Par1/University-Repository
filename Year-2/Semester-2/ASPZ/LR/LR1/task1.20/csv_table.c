/*
 * Task 1.20 - CSV to formatted table (supports comma, semicolon, tab delimiters)
 *
 * Compile: gcc -Wall -o task1_20 csv_table.c
 * Usage:   ./task1_20 file.csv [delimiter]
 *          ./task1_20 data.csv ";"
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ROWS 1000
#define MAX_COLS 50
#define MAX_CELL 256
#define MAX_LINE 4096

/* Print separator line for table */
void print_separator(int max_cols, const int *col_widths) {
    printf("+");
    for (int c = 0; c < max_cols; c++) {
        for (int k = 0; k < col_widths[c] + 2; k++) printf("-");
        printf("+");
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file.csv> [delimiter]\n", argv[0]);
        return 1;
    }

    char delim = ',';
    if (argc > 2) {
        if (strcmp(argv[2], "tab") == 0 || strcmp(argv[2], "\\t") == 0)
            delim = '\t';
        else
            delim = argv[2][0];
    }

    FILE *fp = fopen(argv[1], "r");
    if (!fp) { perror("fopen"); return 1; }

    static char cells[MAX_ROWS][MAX_COLS][MAX_CELL];
    int col_widths[MAX_COLS] = {0};
    int rows = 0, max_cols = 0;

    char line[MAX_LINE];
    while (fgets(line, MAX_LINE, fp) && rows < MAX_ROWS) {
        line[strcspn(line, "\r\n")] = '\0';
        int col = 0;
        char *token = strtok(line, (char[]){delim, '\0'});
        while (token && col < MAX_COLS) {
            /* Trim spaces */
            while (*token == ' ') token++;
            char *end = token + strlen(token) - 1;
            while (end > token && *end == ' ') *end-- = '\0';

            strncpy(cells[rows][col], token, MAX_CELL - 1);
            int len = strlen(cells[rows][col]);
            if (len > col_widths[col]) col_widths[col] = len;

            col++;
            token = strtok(NULL, (char[]){delim, '\0'});
        }
        if (col > max_cols) max_cols = col;
        rows++;
    }
    fclose(fp);

    /* Print table */
    print_separator(max_cols, col_widths);
    for (int r = 0; r < rows; r++) {
        printf("|");
        for (int c = 0; c < max_cols; c++) {
            printf(" %-*s |", col_widths[c], cells[r][c]);
        }
        printf("\n");
        if (r == 0) print_separator(max_cols, col_widths); /* after header */
    }
    print_separator(max_cols, col_widths);

    printf("\n(%d rows, %d columns, delimiter='%c')\n", rows, max_cols,
           delim == '\t' ? 'T' : delim);

    return 0;
}
