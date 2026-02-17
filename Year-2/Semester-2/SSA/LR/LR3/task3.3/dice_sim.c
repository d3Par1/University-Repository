/*
 * Task 3.3: Dice simulator with file size limit (RLIMIT_FSIZE)
 *
 * Simulates rolling a 6-sided die, writes results to a file.
 * Sets a max file size limit and handles SIGXFSZ when exceeded.
 *
 * Compile: gcc -Wall -Wextra -std=c11 dice_sim.c -o dice_sim
 * Run:     ./dice_sim [max_file_bytes] [output_file]
 * Example: ./dice_sim 1024 dice_results.txt
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/resource.h>

static volatile sig_atomic_t file_limit_hit = 0;

static void sigxfsz_handler(int sig) {
    (void)sig;
    file_limit_hit = 1;
}

static int roll_die(void) {
    return (rand() % 6) + 1;
}

int main(int argc, char *argv[]) {
    long max_bytes = 512;          /* Default max file size */
    const char *outfile = "dice_results.txt";

    if (argc >= 2)
        max_bytes = atol(argv[1]);
    if (argc >= 3)
        outfile = argv[2];

    printf("=== Task 3.3: Dice Simulator with File Size Limit ===\n");
    printf("Max file size: %ld bytes\n", max_bytes);
    printf("Output file:   %s\n\n", outfile);

    /* Install SIGXFSZ handler */
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigxfsz_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGXFSZ, &sa, NULL) != 0) {
        perror("sigaction");
        return 1;
    }

    /* Set file size limit */
    struct rlimit rl;
    rl.rlim_cur = (rlim_t)max_bytes;
    rl.rlim_max = (rlim_t)max_bytes;
    if (setrlimit(RLIMIT_FSIZE, &rl) != 0) {
        perror("setrlimit FSIZE");
        return 1;
    }
    printf("RLIMIT_FSIZE set to %ld bytes\n", max_bytes);

    /* Open output file */
    FILE *fp = fopen(outfile, "w");
    if (!fp) {
        perror("fopen");
        return 1;
    }

    /* Seed random generator */
    srand((unsigned int)time(NULL));

    /* Simulate dice rolls and write to file */
    int rolls = 0;
    int histogram[7] = {0}; /* index 1-6 */

    printf("Rolling dice and writing to file...\n");

    while (!file_limit_hit) {
        int die = roll_die();
        histogram[die]++;
        rolls++;

        int ret = fprintf(fp, "Roll #%d: %d\n", rolls, die);
        if (ret < 0 || file_limit_hit) {
            if (errno == EFBIG || file_limit_hit) {
                printf("\n*** File size limit reached! ***\n");
                printf("SIGXFSZ received after %d rolls\n", rolls);
            } else {
                perror("fprintf");
            }
            break;
        }
        fflush(fp);
    }

    fclose(fp);

    /* Print statistics */
    printf("\n--- Dice Statistics ---\n");
    printf("Total rolls: %d\n", rolls);
    printf("Distribution:\n");
    for (int i = 1; i <= 6; i++) {
        printf("  %d: %d times (%.1f%%)\n",
               i, histogram[i],
               rolls > 0 ? (100.0 * histogram[i] / rolls) : 0.0);
    }

    /* Check actual file size */
    FILE *check = fopen(outfile, "r");
    if (check) {
        fseek(check, 0, SEEK_END);
        long size = ftell(check);
        fclose(check);
        printf("\nActual file size: %ld bytes (limit: %ld bytes)\n", size, max_bytes);
    }

    return 0;
}
