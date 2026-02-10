/*
 * Task 3.5: File copy program with error handling and size limit
 *
 * Copies one named file to another. Handles:
 * - Missing arguments
 * - File open errors (read/write)
 * - File size limit (RLIMIT_FSIZE / SIGXFSZ)
 *
 * Compile: gcc -Wall -Wextra -std=c11 filecopy.c -o filecopy
 * Run:     ./filecopy source.txt dest.txt [max_size_bytes]
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <sys/resource.h>

#define BUFFER_SIZE 4096

static volatile sig_atomic_t fsize_exceeded = 0;

static void sigxfsz_handler(int sig) {
    (void)sig;
    fsize_exceeded = 1;
}

int main(int argc, char *argv[]) {
    printf("=== Task 3.5: File Copy with Size Limit ===\n\n");

    /* Check arguments */
    if (argc < 3) {
        fprintf(stderr, "Program need two arguments\n");
        fprintf(stderr, "Usage: %s <source> <dest> [max_size_bytes]\n", argv[0]);
        return 1;
    }

    const char *src_name = argv[1];
    const char *dst_name = argv[2];
    long max_size = 0;

    if (argc >= 4)
        max_size = atol(argv[3]);

    /* Install SIGXFSZ handler */
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigxfsz_handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGXFSZ, &sa, NULL) != 0) {
        perror("sigaction");
        return 1;
    }

    /* Set file size limit if specified */
    if (max_size > 0) {
        struct rlimit rl;
        rl.rlim_cur = (rlim_t)max_size;
        rl.rlim_max = (rlim_t)max_size;
        if (setrlimit(RLIMIT_FSIZE, &rl) != 0) {
            perror("setrlimit FSIZE");
            return 1;
        }
        printf("File size limit set to %ld bytes\n", max_size);
    }

    /* Open source file for reading */
    FILE *src = fopen(src_name, "rb");
    if (!src) {
        fprintf(stderr, "Cannot open file %s for reading\n", src_name);
        return 1;
    }

    /* Open destination file for writing */
    FILE *dst = fopen(dst_name, "wb");
    if (!dst) {
        fprintf(stderr, "Cannot open file %s for writing\n", dst_name);
        fclose(src);
        return 1;
    }

    /* Copy data */
    char buffer[BUFFER_SIZE];
    size_t bytes_read, total_bytes = 0;
    int error_occurred = 0;

    printf("Copying '%s' -> '%s'...\n", src_name, dst_name);

    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, src)) > 0) {
        if (fsize_exceeded) {
            fprintf(stderr, "\n*** File size limit exceeded during copy! ***\n");
            fprintf(stderr, "Copied %zu bytes before limit was reached\n", total_bytes);
            error_occurred = 1;
            break;
        }

        size_t bytes_written = fwrite(buffer, 1, bytes_read, dst);
        if (bytes_written < bytes_read || fsize_exceeded) {
            if (fsize_exceeded || errno == EFBIG) {
                total_bytes += bytes_written;
                fprintf(stderr, "\n*** File size limit exceeded during copy! ***\n");
                fprintf(stderr, "Copied %zu bytes before limit was reached\n", total_bytes);
            } else {
                perror("fwrite");
            }
            error_occurred = 1;
            break;
        }
        total_bytes += bytes_written;
    }

    if (ferror(src)) {
        perror("fread");
        error_occurred = 1;
    }

    fclose(src);
    fclose(dst);

    if (!error_occurred) {
        printf("Successfully copied %zu bytes\n", total_bytes);
    }

    return error_occurred ? 1 : 0;
}
