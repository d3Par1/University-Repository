// Завдання 1.17: Сортування вставками
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef int (*CmpFunc)(const void *, const void *);

int binary_find_pos(void *base, int n, void *key, size_t elem_size, CmpFunc cmp) {
    int lo = 0, hi = n;
    while (lo < hi) {
        int mid = (lo + hi) / 2;
        if (cmp((char *)base + mid * elem_size, key) <= 0)
            lo = mid + 1;
        else
            hi = mid;
    }
    return lo;
}

void insort(void *base, int n, size_t elem_size, CmpFunc cmp) {
    void *key = malloc(elem_size);
    for (int i = 1; i < n; i++) {
        memcpy(key, (char *)base + i * elem_size, elem_size);
        int j = i - 1;
        while (j >= 0 && cmp((char *)base + j * elem_size, key) > 0) {
            memcpy((char *)base + (j + 1) * elem_size,
                   (char *)base + j * elem_size, elem_size);
            j--;
        }
        memcpy((char *)base + (j + 1) * elem_size, key, elem_size);
    }
    free(key);
}

void insort_binary(void *base, int n, size_t elem_size, CmpFunc cmp) {
    void *key = malloc(elem_size);
    for (int i = 1; i < n; i++) {
        memcpy(key, (char *)base + i * elem_size, elem_size);
        int pos = binary_find_pos(base, i, key, elem_size, cmp);
        memmove((char *)base + (pos + 1) * elem_size,
                (char *)base + pos * elem_size,
                (i - pos) * elem_size);
        memcpy((char *)base + pos * elem_size, key, elem_size);
    }
    free(key);
}

typedef struct {
    int priority;
    int value;
} Item;

int cmp_int(const void *a, const void *b) { return *(int *)a - *(int *)b; }

int cmp_item_multi(const void *a, const void *b) {
    const Item *ia = a, *ib = b;
    if (ia->priority != ib->priority)
        return ia->priority - ib->priority;
    return ia->value - ib->value;
}

void print_int_arr(int *a, int n) {
    for (int i = 0; i < n; i++) printf("%d ", a[i]);
    printf("\n");
}

int main() {
    srand(time(NULL));

    printf("=== Task 1.17: Insertion Sort ===\n\n");

    int arr[] = {34, 8, 64, 51, 32, 21, 7, 12, 45, 99};
    int n = sizeof(arr) / sizeof(arr[0]);

    printf("Original: ");
    print_int_arr(arr, n);

    int arr2[10];
    memcpy(arr2, arr, sizeof(arr));

    insort(arr, n, sizeof(int), cmp_int);
    printf("Basic insort: ");
    print_int_arr(arr, n);

    insort_binary(arr2, n, sizeof(int), cmp_int);
    printf("Binary insort: ");
    print_int_arr(arr2, n);

    printf("\n--- Multi-criteria sort ---\n");
    Item items[] = {{3, 10}, {1, 5}, {2, 8}, {1, 3}, {3, 1}, {2, 2}};
    int m = sizeof(items) / sizeof(items[0]);

    printf("Before: ");
    for (int i = 0; i < m; i++) printf("(%d,%d) ", items[i].priority, items[i].value);
    printf("\n");

    insort_binary(items, m, sizeof(Item), cmp_item_multi);

    printf("After:  ");
    for (int i = 0; i < m; i++) printf("(%d,%d) ", items[i].priority, items[i].value);
    printf("\n");

    printf("\n--- Performance (n=10000) ---\n");
    int big_n = 10000;
    int *big1 = malloc(big_n * sizeof(int));
    int *big2 = malloc(big_n * sizeof(int));
    for (int i = 0; i < big_n; i++) big1[i] = big2[i] = rand();

    clock_t t1 = clock();
    insort(big1, big_n, sizeof(int), cmp_int);
    clock_t t2 = clock();
    printf("Basic insort:  %.4f s\n", (double)(t2 - t1) / CLOCKS_PER_SEC);

    t1 = clock();
    insort_binary(big2, big_n, sizeof(int), cmp_int);
    t2 = clock();
    printf("Binary insort: %.4f s\n", (double)(t2 - t1) / CLOCKS_PER_SEC);

    free(big1);
    free(big2);
    return 0;
}
