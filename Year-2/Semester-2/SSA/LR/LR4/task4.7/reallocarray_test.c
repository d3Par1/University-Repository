/*
 * Task 4.7: Rewrite code using reallocarray(3)
 *
 * Original code:
 *   struct sbar *ptr, *newptr;
 *   ptr = calloc(1000, sizeof(struct sbar));
 *   newptr = realloc(ptr, 500 * sizeof(struct sbar));
 *
 * reallocarray(ptr, nmemb, size) = realloc(ptr, nmemb * size)
 * but with overflow checking for nmemb * size multiplication.
 *
 * Compile: gcc -Wall -Wextra -std=c11 -D_GNU_SOURCE reallocarray_test.c -o reallocarray_test
 * Run:     ltrace ./reallocarray_test
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

struct sbar {
    int id;
    char name[32];
    double value;
};

static void print_sbar(const struct sbar *s, int count) {
    for (int i = 0; i < count && i < 5; i++) {
        printf("    [%d] id=%d, name=\"%s\", value=%.2f\n",
               i, s[i].id, s[i].name, s[i].value);
    }
    if (count > 5)
        printf("    ... (%d more)\n", count - 5);
}

int main(void) {
    printf("=== Task 4.7: Using reallocarray(3) ===\n\n");

    printf("sizeof(struct sbar) = %zu bytes\n\n", sizeof(struct sbar));

    /* Original code using calloc + realloc */
    printf("--- Original code (calloc + realloc) ---\n");
    struct sbar *ptr = calloc(1000, sizeof(struct sbar));
    if (!ptr) {
        perror("calloc");
        return 1;
    }
    printf("  calloc(1000, %zu) = %p\n", sizeof(struct sbar), (void *)ptr);
    printf("  Total: %zu bytes\n", 1000 * sizeof(struct sbar));

    /* Initialize some data */
    for (int i = 0; i < 1000; i++) {
        ptr[i].id = i;
        snprintf(ptr[i].name, sizeof(ptr[i].name), "item_%d", i);
        ptr[i].value = i * 1.5;
    }

    struct sbar *newptr = realloc(ptr, 500 * sizeof(struct sbar));
    if (!newptr) {
        perror("realloc");
        free(ptr);
        return 1;
    }
    ptr = newptr;
    printf("  realloc(%zu) = %p\n", 500 * sizeof(struct sbar), (void *)ptr);
    printf("  Data preserved:\n");
    print_sbar(ptr, 500);
    free(ptr);

    /* Rewritten with reallocarray */
    printf("\n--- Rewritten with reallocarray ---\n");
    ptr = calloc(1000, sizeof(struct sbar));
    if (!ptr) {
        perror("calloc");
        return 1;
    }
    printf("  calloc(1000, %zu) = %p\n", sizeof(struct sbar), (void *)ptr);

    for (int i = 0; i < 1000; i++) {
        ptr[i].id = i;
        snprintf(ptr[i].name, sizeof(ptr[i].name), "item_%d", i);
        ptr[i].value = i * 1.5;
    }

    /* reallocarray checks for overflow in nmemb * size */
    newptr = reallocarray(ptr, 500, sizeof(struct sbar));
    if (!newptr) {
        perror("reallocarray");
        free(ptr);
        return 1;
    }
    ptr = newptr;
    printf("  reallocarray(ptr, 500, %zu) = %p\n",
           sizeof(struct sbar), (void *)ptr);
    printf("  Data preserved:\n");
    print_sbar(ptr, 500);
    free(ptr);

    /* Demonstrate overflow protection */
    printf("\n--- Overflow protection ---\n");

    /* With realloc: overflow in multiplication is silent! */
    size_t nmemb_huge = SIZE_MAX / sizeof(struct sbar) + 1;
    printf("  nmemb = %zu, size = %zu\n", nmemb_huge, sizeof(struct sbar));
    printf("  nmemb * size would OVERFLOW to %zu\n",
           nmemb_huge * sizeof(struct sbar));

    printf("\n  realloc approach: nmemb * size overflows silently!\n");
    printf("  realloc(ptr, nmemb * size) might allocate LESS than expected.\n");

    errno = 0;
    void *bad = malloc(nmemb_huge * sizeof(struct sbar)); /* Overflowed size */
    printf("  malloc(overflowed) = %p, errno = %d\n", bad, errno);
    free(bad);

    printf("\n  reallocarray approach: detects overflow, returns NULL + ENOMEM\n");
    void *p = malloc(64);
    errno = 0;
    void *safe = reallocarray(p, nmemb_huge, sizeof(struct sbar));
    printf("  reallocarray(p, huge, %zu) = %p\n", sizeof(struct sbar), safe);
    if (!safe)
        printf("  errno = %d (%s) - overflow detected!\n", errno, strerror(errno));
    free(safe ? safe : p); /* Free whichever is valid */

    /* Growing array example */
    printf("\n--- Growing array with reallocarray ---\n");
    struct sbar *arr = NULL;
    size_t cap = 0;
    size_t len = 0;

    for (int i = 0; i < 20; i++) {
        if (len >= cap) {
            size_t new_cap = cap == 0 ? 4 : cap * 2;
            struct sbar *tmp = reallocarray(arr, new_cap, sizeof(struct sbar));
            if (!tmp) {
                perror("reallocarray");
                break;
            }
            arr = tmp;
            cap = new_cap;
        }
        arr[len].id = i;
        snprintf(arr[len].name, sizeof(arr[len].name), "dynamic_%d", i);
        arr[len].value = i * 3.14;
        len++;
    }

    printf("  Final array: %zu elements, capacity %zu\n", len, cap);
    print_sbar(arr, (int)len);
    free(arr);

    printf("\n=== Comparison ===\n");
    printf("realloc(ptr, n * size):\n");
    printf("  + Works everywhere (C89+)\n");
    printf("  - No overflow check on n * size\n");
    printf("  - Can allocate wrong amount silently\n\n");
    printf("reallocarray(ptr, n, size):\n");
    printf("  + Overflow-safe multiplication\n");
    printf("  + Returns NULL + ENOMEM on overflow\n");
    printf("  - glibc/BSD extension (not C standard)\n");
    printf("  - Use with #define _GNU_SOURCE\n\n");
    printf("Run with ltrace to compare syscalls:\n");
    printf("  ltrace ./reallocarray_test 2>&1 | grep -E 'calloc|realloc|free'\n");

    return 0;
}
