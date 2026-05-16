#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
 * Task 8.3: Find inputs that make qsort slow + test its correctness.
 *
 * glibc's qsort is a hybrid: introsort-style with median-of-3 pivot,
 * falling back to mergesort when stack would overflow. It is MUCH more
 * robust than textbook quicksort. But certain patterns still slow it down:
 *
 *   - All equal elements: most quicksorts handle this poorly because every
 *     element compares equal to the pivot.
 *   - Median-of-3 killer sequences (Musser 1997).
 *   - Already-sorted (textbook qsort is O(n^2); glibc's is O(n log n)).
 *
 * Our test compares times on:
 *   A) random data
 *   B) all-zero data
 *   C) sorted ascending
 *   D) sorted descending
 * and verifies the result is actually sorted in each case.
 */

static int cmp_int(const void *a, const void *b)
{
    int x = *(const int *)a, y = *(const int *)b;
    /* return x - y;   <-- UNSAFE for INT_MIN due to overflow */
    return (x > y) - (x < y);
}

static int sorted(const int *a, size_t n)
{
    for (size_t i = 1; i < n; ++i) if (a[i - 1] > a[i]) return 0;
    return 1;
}

static double measure(const char *label, int *base, size_t n)
{
    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    qsort(base, n, sizeof(int), cmp_int);
    clock_gettime(CLOCK_MONOTONIC, &t1);
    double ms = (t1.tv_sec - t0.tv_sec) * 1e3 + (t1.tv_nsec - t0.tv_nsec) / 1e6;
    printf("%-25s %8.3f ms   sorted=%s\n", label, ms, sorted(base, n) ? "yes" : "NO!");
    return ms;
}

int main(void)
{
    enum { N = 200000 };
    int *a = malloc(N * sizeof(int));
    int *b = malloc(N * sizeof(int));
    int *c = malloc(N * sizeof(int));
    int *d = malloc(N * sizeof(int));

    srand(42);
    for (int i = 0; i < N; ++i) {
        a[i] = rand();
        b[i] = 0;
        c[i] = i;
        d[i] = N - i;
    }

    printf("Sorting %d ints with qsort (glibc):\n", N);
    measure("random          ",  a, N);
    measure("all zero        ",  b, N);
    measure("already sorted  ",  c, N);
    measure("reverse sorted  ",  d, N);

    free(a); free(b); free(c); free(d);
    return 0;
}
