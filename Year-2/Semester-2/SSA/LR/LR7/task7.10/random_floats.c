#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

/*
 * Task 7.10: Generate random floats in (a) [0,1] and (b) [0,n].
 * Seed is mixed from clock_gettime nanoseconds + PID so two parallel runs
 * never produce the same sequence (time(NULL) alone has 1-second resolution
 * which is not enough for parallel processes).
 */
static void seed_rand(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    unsigned seed = (unsigned)(ts.tv_nsec ^ (ts.tv_sec << 16) ^ (getpid() * 2654435761u));
    srand(seed);
    fprintf(stderr, "seed=%u\n", seed);
}

int main(int argc, char **argv)
{
    seed_rand();
    int count = argc > 1 ? atoi(argv[1]) : 5;
    double n = argc > 2 ? atof(argv[2]) : 100.0;

    puts("(a) uniform [0, 1]:");
    for (int i = 0; i < count; ++i)
        printf("  %.6f\n", (double)rand() / RAND_MAX);

    printf("(b) uniform [0, %g]:\n", n);
    for (int i = 0; i < count; ++i)
        printf("  %.6f\n", (double)rand() / RAND_MAX * n);
    return 0;
}
