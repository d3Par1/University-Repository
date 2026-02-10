/* Task 1.4 - module1.c: Parallel sum using pthreads */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 4

typedef struct {
    int start;
    int end;
    double result;
} ThreadArg;

static void *sum_worker(void *arg) {
    ThreadArg *ta = (ThreadArg *)arg;
    ta->result = 0.0;
    for (int i = ta->start; i < ta->end; i++) {
        ta->result += 1.0 / (i + 1);
    }
    return NULL;
}

void compute_sum_pthread(int n) {
    pthread_t threads[NUM_THREADS];
    ThreadArg args[NUM_THREADS];
    int chunk = n / NUM_THREADS;

    for (int i = 0; i < NUM_THREADS; i++) {
        args[i].start = i * chunk;
        args[i].end = (i == NUM_THREADS - 1) ? n : (i + 1) * chunk;
        pthread_create(&threads[i], NULL, sum_worker, &args[i]);
    }

    double total = 0.0;
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
        total += args[i].result;
    }

    printf("[pthreads] Harmonic sum (n=%d, threads=%d) = %.10f\n",
           n, NUM_THREADS, total);
}
