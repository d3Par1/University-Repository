#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
 * Task 7.9: Measure execution time of a code fragment in milliseconds.
 *
 * clock_gettime(CLOCK_MONOTONIC) is preferred over `clock()` because:
 *  - clock() measures CPU time, not wall time — multithreaded code lies.
 *  - CLOCK_MONOTONIC never jumps backwards (NTP adjustments leave it alone).
 *  - Nanosecond resolution vs 10ms tick resolution of clock().
 */
static double elapsed_ms(struct timespec a, struct timespec b)
{
    return (b.tv_sec - a.tv_sec) * 1000.0
         + (b.tv_nsec - a.tv_nsec) / 1e6;
}

int main(void)
{
    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);

    /* the fragment under test: O(n^2) busy loop */
    volatile long sum = 0;
    for (long i = 0; i < 200000; ++i)
        for (long j = 0; j < 200; ++j)
            sum += i * j;

    clock_gettime(CLOCK_MONOTONIC, &t1);
    printf("inner sum=%ld\n", sum);
    printf("elapsed: %.3f ms\n", elapsed_ms(t0, t1));
    return 0;
}
