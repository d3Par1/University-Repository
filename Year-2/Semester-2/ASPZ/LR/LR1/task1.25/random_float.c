/*
 * Task 1.25 - Random float generator with seed support
 *
 * Compile: gcc -Wall -o task1_25 random_float.c
 * Usage:   ./task1_25 100       (random float in [0, 100])
 *          ./task1_25 100 42    (with seed=42)
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double random_float(double max_val) {
    return ((double)rand() / RAND_MAX) * max_val;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <max_value> [seed]\n", argv[0]);
        return 1;
    }

    double max_val = atof(argv[1]);
    unsigned int seed;

    if (argc > 2) {
        seed = (unsigned int)atoi(argv[2]);
        printf("Using seed: %u\n", seed);
    } else {
        seed = (unsigned int)time(NULL);
        printf("Using time-based seed: %u\n", seed);
    }
    srand(seed);

    printf("Range: [0, %.2f]\n\n", max_val);

    /* Generate 10 random numbers */
    for (int i = 0; i < 10; i++) {
        printf("  [%d] %.6f\n", i, random_float(max_val));
    }

    /* Demonstrate reproducibility with same seed */
    printf("\n--- Reproducibility test (seed=%u) ---\n", seed);
    srand(seed);
    printf("  First 3: %.6f, %.6f, %.6f\n",
           random_float(max_val), random_float(max_val), random_float(max_val));
    srand(seed);
    printf("  Again:   %.6f, %.6f, %.6f\n",
           random_float(max_val), random_float(max_val), random_float(max_val));

    return 0;
}
