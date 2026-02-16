/*
 * Task 5.7: Buffer overflow demonstration
 *
 * Writing beyond a buffer's boundary can overwrite adjacent
 * variables, return addresses, or heap metadata — leading to
 * crashes or security exploits.
 *
 * Compile: gcc -Wall -Wextra -std=c11 -g -fno-stack-protector buffer_overflow.c -o buffer_overflow
 * Run:     ./buffer_overflow
 * Valgrind: valgrind ./buffer_overflow
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Demo 1: Stack buffer overflow overwrites adjacent variable */
static void demo_stack_overflow(void) {
    printf("--- Demo 1: Stack buffer overflow ---\n");

    /* Layout on stack (order may vary by compiler): */
    int after = 0xBEEF;
    char buf[8];
    int before = 0xCAFE;

    memset(buf, 0, sizeof(buf));
    printf("  Before overflow:\n");
    printf("    before = 0x%X, after = 0x%X\n", before, after);
    printf("    buf[8] at %p\n", (void *)buf);

    /* Overflow: write 16 bytes into 8-byte buffer */
    memset(buf, 'A', 16);
    printf("  After memset(buf, 'A', 16):\n");
    printf("    before = 0x%X, after = 0x%X\n", before, after);
    printf("    One of them may be corrupted (0x41414141 = 'AAAA')\n\n");
}

/* Demo 2: Heap buffer overflow */
static void demo_heap_overflow(void) {
    printf("--- Demo 2: Heap buffer overflow ---\n");
    char *a = malloc(8);
    char *b = malloc(8);

    if (!a || !b) {
        free(a);
        free(b);
        return;
    }

    strcpy(a, "AAAA");
    strcpy(b, "BBBB");
    printf("  Before: a=\"%s\", b=\"%s\"\n", a, b);

    /* Overflow a — may corrupt malloc metadata between a and b */
    memset(a, 'X', 24); /* writes far beyond 8-byte allocation */
    printf("  After memset(a, 'X', 24):\n");
    printf("    a's content overflows into heap metadata and b's block\n");
    printf("    Valgrind: Invalid write of size 1\n\n");

    /* Don't free — heap may be corrupted */
}

/* Demo 3: strcpy without bounds checking */
static void demo_strcpy_overflow(void) {
    printf("--- Demo 3: strcpy overflow ---\n");
    char dest[8];
    const char *src = "This string is way too long for dest";

    printf("  dest[8], src has %zu bytes\n", strlen(src) + 1);
    /* strcpy doesn't check bounds — classic vulnerability */
    strncpy(dest, src, sizeof(dest) - 1);
    dest[sizeof(dest) - 1] = '\0';
    printf("  Safe version with strncpy: \"%s\"\n", dest);
    printf("  strcpy(dest, src) would overflow the buffer!\n\n");
}

int main(void) {
    printf("=== Task 5.7: Buffer Overflow ===\n\n");

    printf("Buffer overflow: writing beyond allocated memory.\n");
    printf("Stack overflow can overwrite return address (code execution).\n");
    printf("Heap overflow can corrupt allocator metadata.\n\n");

    demo_stack_overflow();
    demo_heap_overflow();
    demo_strcpy_overflow();

    printf("=== Summary ===\n");
    printf("Prevention:\n");
    printf("  - Use strncpy/snprintf instead of strcpy/sprintf\n");
    printf("  - Check buffer sizes before writing\n");
    printf("  - Compile with -fstack-protector (detects stack smashing)\n");
    printf("  - Use AddressSanitizer: -fsanitize=address\n");

    return 0;
}
