// Завдання 1.15: Алгоритми сортування
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>

int cmp_asc(const void *a, const void *b) { return *(int *)a - *(int *)b; }
int cmp_desc(const void *a, const void *b) { return *(int *)b - *(int *)a; }

void merge(int *arr, int *tmp, int left, int mid, int right) {
    int i = left, j = mid + 1, k = left;
    while (i <= mid && j <= right) {
        tmp[k++] = (arr[i] <= arr[j]) ? arr[i++] : arr[j++];
    }
    while (i <= mid) tmp[k++] = arr[i++];
    while (j <= right) tmp[k++] = arr[j++];
    memcpy(arr + left, tmp + left, (right - left + 1) * sizeof(int));
}

void merge_sort_rec(int *arr, int *tmp, int left, int right) {
    if (left >= right) return;
    int mid = (left + right) / 2;
    merge_sort_rec(arr, tmp, left, mid);
    merge_sort_rec(arr, tmp, mid + 1, right);
    merge(arr, tmp, left, mid, right);
}

void merge_sort(int *arr, int n) {
    int *tmp = malloc(n * sizeof(int));
    merge_sort_rec(arr, tmp, 0, n - 1);
    free(tmp);
}

void heapify(int *arr, int n, int i) {
    int largest = i, left = 2 * i + 1, right = 2 * i + 2;
    if (left < n && arr[left] > arr[largest]) largest = left;
    if (right < n && arr[right] > arr[largest]) largest = right;
    if (largest != i) {
        int tmp = arr[i]; arr[i] = arr[largest]; arr[largest] = tmp;
        heapify(arr, n, largest);
    }
}

void heap_sort(int *arr, int n) {
    for (int i = n / 2 - 1; i >= 0; i--) heapify(arr, n, i);
    for (int i = n - 1; i > 0; i--) {
        int tmp = arr[0]; arr[0] = arr[i]; arr[i] = tmp;
        heapify(arr, i, 0);
    }
}

void parallel_merge_sort(int *arr, int *tmp, int left, int right, int depth) {
    if (left >= right) return;
    int mid = (left + right) / 2;

    if (depth < 3) {
        #pragma omp parallel sections
        {
            #pragma omp section
            parallel_merge_sort(arr, tmp, left, mid, depth + 1);
            #pragma omp section
            parallel_merge_sort(arr, tmp, mid + 1, right, depth + 1);
        }
    } else {
        merge_sort_rec(arr, tmp, left, mid);
        merge_sort_rec(arr, tmp, mid + 1, right);
    }
    merge(arr, tmp, left, mid, right);
}

void print_array(int *arr, int n) {
    int limit = (n > 20) ? 20 : n;
    for (int i = 0; i < limit; i++) printf("%d ", arr[i]);
    if (n > 20) printf("...");
    printf("\n");
}

int main(int argc, char *argv[]) {
    char *method = "qsort";
    int n = 100000;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-t") == 0 && i + 1 < argc) method = argv[++i];
        else n = atoi(argv[i]);
    }

    printf("=== Task 1.15: Sorting (%s, n=%d) ===\n\n", method, n);

    int *arr = malloc(n * sizeof(int));
    srand(time(NULL));
    for (int i = 0; i < n; i++) arr[i] = rand() % 100000;

    printf("Before: ");
    print_array(arr, n);

    double start = omp_get_wtime();

    if (strcmp(method, "merge") == 0) {
        merge_sort(arr, n);
    } else if (strcmp(method, "heap") == 0) {
        heap_sort(arr, n);
    } else if (strcmp(method, "parallel") == 0) {
        int *tmp = malloc(n * sizeof(int));
        parallel_merge_sort(arr, tmp, 0, n - 1, 0);
        free(tmp);
    } else {
        qsort(arr, n, sizeof(int), cmp_asc);
    }

    double elapsed = omp_get_wtime() - start;

    printf("After:  ");
    print_array(arr, n);
    printf("Time: %.6f seconds\n", elapsed);

    free(arr);
    return 0;
}
