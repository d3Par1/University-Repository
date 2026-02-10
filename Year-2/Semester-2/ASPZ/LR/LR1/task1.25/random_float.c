// Завдання 1.25: Генератор випадкових чисел
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

    for (int i = 0; i < 10; i++) {
        printf("  [%d] %.6f\n", i, random_float(max_val));
    }

    printf("\n--- Reproducibility test (seed=%u) ---\n", seed);
    srand(seed);
    printf("  First 3: %.6f, %.6f, %.6f\n",
           random_float(max_val), random_float(max_val), random_float(max_val));
    srand(seed);
    printf("  Again:   %.6f, %.6f, %.6f\n",
           random_float(max_val), random_float(max_val), random_float(max_val));

    return 0;
}
