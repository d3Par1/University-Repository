/*
 * Task 1.16 - Struct sort by keyword, multi-field, bsearch
 *
 * Compile: gcc -Wall -o task1_16 struct_sort.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char keyword[64];
    int frequency;
} Entry;

/* Comparison functions (function pointers for dynamic selection) */
int cmp_by_keyword(const void *a, const void *b) {
    return strcmp(((Entry *)a)->keyword, ((Entry *)b)->keyword);
}

int cmp_by_frequency(const void *a, const void *b) {
    return ((Entry *)a)->frequency - ((Entry *)b)->frequency;
}

int cmp_by_keyword_then_freq(const void *a, const void *b) {
    int r = strcmp(((Entry *)a)->keyword, ((Entry *)b)->keyword);
    if (r != 0) return r;
    return ((Entry *)a)->frequency - ((Entry *)b)->frequency;
}

typedef int (*CompareFunc)(const void *, const void *);

void print_entries(const Entry *arr, int n) {
    printf("%-20s %s\n", "Keyword", "Frequency");
    printf("-----------------------------\n");
    for (int i = 0; i < n; i++) {
        printf("%-20s %d\n", arr[i].keyword, arr[i].frequency);
    }
}

int main() {
    Entry data[] = {
        {"algorithm", 15}, {"database", 8}, {"network", 12},
        {"algorithm", 3}, {"compiler", 20}, {"database", 5},
        {"network", 7}, {"kernel", 18}, {"compiler", 10},
        {"kernel", 2}, {"algorithm", 25}
    };
    int n = sizeof(data) / sizeof(data[0]);

    printf("=== Task 1.16: Struct Sort ===\n\n");

    printf("--- Original ---\n");
    print_entries(data, n);

    /* Dynamic sort selection via function pointer */
    CompareFunc comparators[] = {cmp_by_keyword, cmp_by_frequency, cmp_by_keyword_then_freq};
    const char *names[] = {"by keyword", "by frequency", "by keyword then frequency"};

    for (int s = 0; s < 3; s++) {
        /* Make a copy */
        Entry *copy = malloc(n * sizeof(Entry));
        memcpy(copy, data, n * sizeof(Entry));

        qsort(copy, n, sizeof(Entry), comparators[s]);
        printf("\n--- Sorted %s ---\n", names[s]);
        print_entries(copy, n);

        /* Binary search (only on keyword-sorted) */
        if (s == 0 || s == 2) {
            Entry key = {"kernel", 0};
            Entry *found = bsearch(&key, copy, n, sizeof(Entry), cmp_by_keyword);
            if (found) {
                printf("bsearch for \"kernel\": found (freq=%d)\n", found->frequency);
            } else {
                printf("bsearch for \"kernel\": not found\n");
            }
        }

        free(copy);
    }

    return 0;
}
