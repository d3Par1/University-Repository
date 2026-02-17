// Завдання 1.4: Багатопоточність (pthreads, OpenMP)
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#ifdef _OPENMP
#include <omp.h>
#endif

void compute_sum_pthread(int n);
void compute_product_pthread(int n);

#define ARRAY_SIZE 10000000

#ifdef _OPENMP
void compute_openmp(void) {
    double sum = 0.0;
    int i;

    #pragma omp parallel for reduction(+:sum)
    for (i = 0; i < ARRAY_SIZE; i++) {
        sum += 1.0 / (i + 1);
    }

    printf("[OpenMP] Harmonic sum (n=%d) = %.10f (threads=%d)\n",
           ARRAY_SIZE, sum, omp_get_max_threads());
}
#endif

int main(int argc, char *argv[]) {
    printf("=== Task 1.4: Multithreading Demo ===\n\n");

    int n = ARRAY_SIZE;
    if (argc > 1) n = atoi(argv[1]);

    printf("--- pthreads ---\n");
    compute_sum_pthread(n);
    compute_product_pthread(n);

    #ifdef _OPENMP
    printf("\n--- OpenMP ---\n");
    compute_openmp();
    #else
    printf("\n--- OpenMP ---\n");
    printf("(OpenMP not available, compile with -fopenmp)\n");
    #endif

    printf("\nDone.\n");
    return 0;
}
