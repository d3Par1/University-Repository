/*
 * Task 2.1 (General) - When does time_t overflow? 32-bit vs 64-bit
 * Also explores ELF segments with size command
 *
 * Compile:
 *   gcc -Wall -m64 -o time_end_64 time_end.c
 *   gcc -Wall -m32 -o time_end_32 time_end.c  (needs gcc-multilib)
 *
 * After compile, run: size time_end_64; size time_end_32
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <stdint.h>

int main() {
    printf("=== Task 2.1: time_t Overflow & Segments ===\n\n");

    printf("--- System Info ---\n");
    printf("sizeof(time_t) = %zu bytes (%zu bits)\n", sizeof(time_t), sizeof(time_t) * 8);
    printf("sizeof(int)    = %zu bytes\n", sizeof(int));
    printf("sizeof(long)   = %zu bytes\n", sizeof(long));
    printf("sizeof(void*)  = %zu bytes (pointer size = %s)\n",
           sizeof(void *), sizeof(void *) == 8 ? "64-bit" : "32-bit");

    printf("\n--- time_t Range ---\n");

    if (sizeof(time_t) == 4) {
        /* 32-bit time_t */
        time_t max_time = (time_t)2147483647; /* INT32_MAX */
        struct tm *tm_info = gmtime(&max_time);
        char buf[64];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S UTC", tm_info);
        printf("32-bit time_t max: %s\n", buf);
        printf("This is the Y2038 problem!\n");
        printf("Max value: %ld (0x%lX)\n", (long)max_time, (unsigned long)max_time);
    } else {
        /* 64-bit time_t */
        printf("64-bit time_t - will not overflow for billions of years\n");
        printf("Max value: %ld\n", (long)INT64_MAX);

        /* Show 32-bit equivalent */
        time_t t32_max = (time_t)2147483647;
        struct tm *tm_info = gmtime(&t32_max);
        char buf[64];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S UTC", tm_info);
        printf("32-bit equivalent max: %s (Y2038)\n", buf);

        /* Current time */
        time_t now = time(NULL);
        tm_info = localtime(&now);
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm_info);
        printf("Current time: %s (value: %ld)\n", buf, (long)now);

        /* Time remaining until 32-bit overflow */
        long remaining = (long)(2147483647 - now);
        printf("Seconds until Y2038: %ld (%.1f years)\n",
               remaining, remaining / 365.25 / 86400);
    }

    printf("\n--- Segment Analysis ---\n");
    printf("Run these commands to see segments:\n");
    printf("  size ./time_end_64\n");
    printf("  readelf -S ./time_end_64\n");
    printf("  objdump -h ./time_end_64\n");

    return 0;
}
