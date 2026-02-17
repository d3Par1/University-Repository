/*
 * Task 3.4: Lottery simulator with CPU time limit (RLIMIT_CPU)
 *
 * Generates lottery numbers: 7 from 49 and 6 from 36.
 * Sets CPU time limit and handles SIGXCPU when exceeded.
 *
 * Compile: gcc -Wall -Wextra -std=c11 lottery.c -o lottery
 * Run:     ./lottery [cpu_seconds]
 * Example: ./lottery 2
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/resource.h>

static volatile sig_atomic_t cpu_limit_hit = 0;

static void sigxcpu_handler(int sig) {
    (void)sig;
    cpu_limit_hit = 1;
}

/* Generate unique random numbers in range [1, max] */
static void generate_unique(int *arr, int count, int max) {
    int *pool = calloc((size_t)max + 1, sizeof(int));
    if (!pool) {
        perror("calloc");
        return;
    }

    int generated = 0;
    while (generated < count) {
        int num = (rand() % max) + 1;
        if (!pool[num]) {
            pool[num] = 1;
            arr[generated++] = num;
        }
    }

    /* Sort for nice output */
    for (int i = 0; i < count - 1; i++)
        for (int j = i + 1; j < count; j++)
            if (arr[i] > arr[j]) {
                int tmp = arr[i];
                arr[i] = arr[j];
                arr[j] = tmp;
            }

    free(pool);
}

static void print_numbers(const char *label, const int *arr, int count) {
    printf("  %s: ", label);
    for (int i = 0; i < count; i++) {
        if (i > 0) printf(", ");
        printf("%2d", arr[i]);
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    int cpu_limit = 3; /* Default CPU seconds */

    if (argc >= 2)
        cpu_limit = atoi(argv[1]);

    printf("=== Task 3.4: Lottery Simulator with CPU Time Limit ===\n");
    printf("CPU time limit: %d seconds\n\n", cpu_limit);

    /* Install SIGXCPU handler */
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigxcpu_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGXCPU, &sa, NULL) != 0) {
        perror("sigaction");
        return 1;
    }

    /* Set CPU time limit */
    struct rlimit rl;
    rl.rlim_cur = (rlim_t)cpu_limit;
    rl.rlim_max = (rlim_t)(cpu_limit + 1); /* Hard limit 1 sec more */
    if (setrlimit(RLIMIT_CPU, &rl) != 0) {
        perror("setrlimit CPU");
        return 1;
    }
    printf("RLIMIT_CPU set: soft=%d, hard=%d seconds\n\n", cpu_limit, cpu_limit + 1);

    srand((unsigned int)time(NULL));

    int draw_count = 0;
    int nums7[7], nums6[6];

    printf("Generating lottery draws...\n\n");

    while (!cpu_limit_hit) {
        generate_unique(nums7, 7, 49);
        generate_unique(nums6, 6, 36);
        draw_count++;

        if (draw_count <= 10 || draw_count % 100000 == 0) {
            printf("Draw #%d:\n", draw_count);
            print_numbers("7 of 49", nums7, 7);
            print_numbers("6 of 36", nums6, 6);
            printf("\n");
        }

        if (cpu_limit_hit) {
            printf("\n*** SIGXCPU received! CPU time limit reached ***\n");
            break;
        }
    }

    /* Show resource usage */
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        printf("\n--- Resource Usage ---\n");
        printf("User CPU time:   %ld.%06ld sec\n",
               usage.ru_utime.tv_sec, usage.ru_utime.tv_usec);
        printf("System CPU time: %ld.%06ld sec\n",
               usage.ru_stime.tv_sec, usage.ru_stime.tv_usec);
    }

    printf("Total draws generated: %d\n", draw_count);
    printf("\nNote: SIGXCPU is sent when soft limit is reached.\n");
    printf("      SIGKILL is sent when hard limit is reached.\n");

    return 0;
}
