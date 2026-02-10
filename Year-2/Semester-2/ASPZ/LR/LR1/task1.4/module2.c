/* Task 1.4 - module2.c: Parallel product using pthreads + async I/O */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>

#define NUM_THREADS 4

typedef struct {
    int start;
    int end;
    double result;
} ThreadArg;

static void *product_worker(void *arg) {
    ThreadArg *ta = (ThreadArg *)arg;
    ta->result = 1.0;
    for (int i = ta->start; i < ta->end && i < 20; i++) {
        ta->result *= (i + 1);
    }
    return NULL;
}

void compute_product_pthread(int n) {
    if (n > 20) n = 20; /* limit to avoid overflow */

    pthread_t threads[NUM_THREADS];
    ThreadArg args[NUM_THREADS];
    int chunk = n / NUM_THREADS;
    if (chunk < 1) chunk = 1;

    int t_count = (n < NUM_THREADS) ? n : NUM_THREADS;
    for (int i = 0; i < t_count; i++) {
        args[i].start = i * chunk;
        args[i].end = (i == t_count - 1) ? n : (i + 1) * chunk;
        pthread_create(&threads[i], NULL, product_worker, &args[i]);
    }

    double total = 1.0;
    for (int i = 0; i < t_count; i++) {
        pthread_join(threads[i], NULL);
        total *= args[i].result;
    }

    printf("[pthreads] Factorial-like product (n=%d) = %.0f\n", n, total);

    /* Write result to file using standard I/O */
    FILE *f = fopen("/tmp/task1_4_result.txt", "w");
    if (f) {
        fprintf(f, "Product result: %.0f\n", total);
        fclose(f);
        printf("[I/O] Result written to /tmp/task1_4_result.txt\n");
    }
}
