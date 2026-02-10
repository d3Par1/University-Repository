/*
 * Task 1.4 - Multithreaded program with pthreads, OpenMP, and std C
 *
 * Compile (pthreads):  gcc -Wall -Wextra -pthread -o task1_4 main.c module1.c module2.c
 * Compile (OpenMP):    gcc -Wall -Wextra -fopenmp -o task1_4_omp main.c module1.c module2.c
 * Valgrind:            valgrind --tool=callgrind ./task1_4
 * Profile:             gcc -Wall -pg -o task1_4_prof main.c module1.c module2.c -pthread && ./task1_4_prof && gprof task1_4_prof gmon.out
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#ifdef _OPENMP
#include <omp.h>
#endif

/* From module1.c */
void compute_sum_pthread(int n);
/* From module2.c */
void compute_product_pthread(int n);

#define ARRAY_SIZE 10000000

#ifdef _OPENMP
/* OpenMP version of parallel computation */
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
