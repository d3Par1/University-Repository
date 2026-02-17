/*
 * Task 5.8: Buffer overflow
 *
 * Writing past the end of a buffer overwrites adjacent memory.
 * On the stack this can corrupt return addresses (classic exploit).
 * On the heap it corrupts allocator metadata or other data.
 *
 * Compile: gcc -Wall -Wextra -std=c11 -g -fno-stack-protector buffer_overflow.c -o buffer_overflow
 * Run:     ./buffer_overflow
 * Valgrind: valgrind ./buffer_overflow
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Stack buffer overflow — corrupts adjacent variable */
static void demo_stack_overflow(void) {
    printf("--- Stack buffer overflow ---\n");

    int canary = 0xCAFEBABE;
    char buf[8];

    printf("  Before overflow: canary = 0x%X\n", canary);
    printf("  buf[8] at %p, canary at %p\n", (void *)buf, (void *)&canary);

    /* Write more than 8 bytes into buf — overflows into canary */
    memset(buf, 'A', 20); /* writes 20 bytes into 8-byte buffer */

    printf("  After overflow:  canary = 0x%X", canary);
    if (canary != (int)0xCAFEBABE)
        printf(" (CORRUPTED!)");
    printf("\n\n");
}

/* Heap buffer overflow */
static void demo_heap_overflow(void) {
    printf("--- Heap buffer overflow ---\n");

    char *buf = malloc(8);
    char *secret = malloc(16);
    if (!buf || !secret) return;

    strcpy(secret, "SECRET_DATA");
    printf("  buf at %p, secret at %p\n", (void *)buf, (void *)secret);
    printf("  secret = \"%s\"\n", secret);

    /* Overflow buf — may corrupt heap metadata or secret */
    memset(buf, 'X', 32); /* writes 32 bytes into 8-byte buffer */
    printf("  After overflow: secret = \"%s\"", secret);
    if (strcmp(secret, "SECRET_DATA") != 0)
        printf(" (CORRUPTED!)");
    printf("\n");
    printf("  Valgrind: Invalid write of size 32\n\n");

    free(buf);
    free(secret);
}

/* Safe version using bounds-checked functions */
static void demo_safe_copy(void) {
    printf("--- Safe version: bounds checking ---\n");

    char buf[8];
    const char *input = "This is a very long input string";

    /* strncpy limits bytes copied */
    strncpy(buf, input, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    printf("  Input: \"%s\"\n", input);
    printf("  buf:   \"%s\" (truncated safely)\n\n", buf);
}

int main(void) {
    printf("=== Task 5.8: Buffer Overflow ===\n\n");

    demo_stack_overflow();
    demo_heap_overflow();
    demo_safe_copy();

    printf("=== Summary ===\n");
    printf("Buffer overflows can corrupt data, crash, or be exploited.\n");
    printf("Always check bounds. Use strncpy, snprintf, or sizeof checks.\n");
    printf("Compile with -fstack-protector for stack canary detection.\n");

    return 0;
}
