/*
 * Task 4.5: What happens when realloc(3) cannot allocate memory?
 *
 * If realloc fails, it returns NULL but does NOT free the original pointer.
 * Common bug: ptr = realloc(ptr, new_size) - if it fails, ptr is lost (leak).
 *
 * Compile: gcc -Wall -Wextra -std=c11 realloc_fail.c -o realloc_fail
 * Run:     ./realloc_fail
 * ltrace:  ltrace ./realloc_fail
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/resource.h>

int main(void) {
    printf("=== Task 4.5: realloc failure behavior ===\n\n");

    /* Test 1: Normal realloc */
    printf("--- Test 1: Normal realloc ---\n");
    char *ptr = malloc(100);
    if (!ptr) {
        perror("malloc");
        return 1;
    }
    strcpy(ptr, "Hello, World!");
    printf("  Original: ptr = %p, content = \"%s\"\n", (void *)ptr, ptr);

    char *new_ptr = realloc(ptr, 200);
    if (new_ptr) {
        ptr = new_ptr;
        printf("  After realloc(200): ptr = %p, content = \"%s\"\n", (void *)ptr, ptr);
    } else {
        printf("  realloc failed! Original ptr still valid at %p\n", (void *)ptr);
    }
    free(ptr);

    /* Test 2: Force realloc failure with huge size */
    printf("\n--- Test 2: realloc with huge size ---\n");
    ptr = malloc(100);
    if (!ptr) {
        perror("malloc");
        return 1;
    }
    strcpy(ptr, "Important data");
    printf("  Original: ptr = %p, content = \"%s\"\n", (void *)ptr, ptr);

    errno = 0;
    /* Try absurdly large realloc */
    new_ptr = realloc(ptr, (size_t)-1 / 2);
    printf("  realloc(SIZE_MAX/2) = %p\n", (void *)new_ptr);
    if (!new_ptr) {
        printf("  realloc FAILED (errno=%d: %s)\n", errno, strerror(errno));
        printf("  Original ptr still valid: \"%s\"\n", ptr);
        /* Original data is preserved! */
    }
    free(ptr); /* Always free original if realloc fails */

    /* Test 3: Buggy pattern - losing original pointer */
    printf("\n--- Test 3: Buggy pattern (DON'T DO THIS) ---\n");
    printf("  /* BUGGY CODE: */\n");
    printf("  ptr = realloc(ptr, huge_size);\n");
    printf("  /* If realloc returns NULL, we lost the original ptr! */\n");
    printf("  /* This is a memory leak. */\n\n");

    printf("  /* CORRECT CODE: */\n");
    printf("  tmp = realloc(ptr, new_size);\n");
    printf("  if (tmp) {\n");
    printf("      ptr = tmp;  /* Success */\n");
    printf("  } else {\n");
    printf("      /* Handle failure, ptr still valid */\n");
    printf("      free(ptr); /* Or continue using ptr */\n");
    printf("  }\n");

    /* Test 4: Repeated realloc growth until failure */
    printf("\n--- Test 4: Growing until realloc fails ---\n");
    size_t size = 1024;
    ptr = malloc(size);
    if (!ptr) {
        perror("malloc");
        return 1;
    }

    int iterations = 0;
    while (1) {
        size_t new_size = size * 2;
        new_ptr = realloc(ptr, new_size);
        if (!new_ptr) {
            printf("  realloc failed at size %zu bytes (%.2f GB)\n",
                   new_size, (double)new_size / (1024 * 1024 * 1024));
            printf("  After %d doublings\n", iterations);
            break;
        }
        ptr = new_ptr;
        size = new_size;
        iterations++;

        if (iterations % 5 == 0)
            printf("  Size: %zu bytes (%.2f MB)\n",
                   size, (double)size / (1024 * 1024));
    }
    free(ptr);

    printf("\n=== Summary ===\n");
    printf("When realloc fails:\n");
    printf("  1. Returns NULL\n");
    printf("  2. Original pointer remains valid\n");
    printf("  3. Original data is not modified or freed\n");
    printf("  4. Always use a temp pointer for realloc result\n");

    return 0;
}
