/*
 * Variant 15: Intercept malloc/free via wrapper and log all allocations.
 *             Detect unfreed blocks (memory leaks).
 *
 * Uses the GNU ld --wrap option to intercept malloc() and free().
 * __wrap_malloc() logs every allocation (address, size).
 * __wrap_free() logs every deallocation.
 * At program exit, atexit() handler reports all unfreed blocks.
 *
 * Compile: gcc -Wall -Wextra -std=c11 -O2 malloc_wrapper.c -o malloc_wrapper \
 *          -Wl,--wrap=malloc,--wrap=free
 * Run:     ./malloc_wrapper
 * ltrace:  ltrace ./malloc_wrapper 2>&1 | head -40
 * Valgrind: valgrind --leak-check=full ./malloc_wrapper
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

/* ---- Allocation record ---- */
typedef struct AllocRecord {
    void   *addr;
    size_t  size;
    struct AllocRecord *next;
} AllocRecord;

/* Global tracking state */
static AllocRecord *alloc_list = NULL;
static size_t alloc_count  = 0;
static size_t free_count   = 0;
static size_t total_bytes  = 0;

/* Real libc functions provided by the linker */
extern void *__real_malloc(size_t size);
extern void  __real_free(void *ptr);

/* ---- Internal: record a new allocation ---- */
static void record_alloc(void *addr, size_t size)
{
    /* Use __real_malloc to avoid recursion */
    AllocRecord *rec = (AllocRecord *)__real_malloc(sizeof(AllocRecord));
    if (!rec) {
        fprintf(stderr, "[WRAPPER] Failed to allocate tracking record!\n");
        return;
    }
    rec->addr = addr;
    rec->size = size;
    rec->next = alloc_list;
    alloc_list = rec;

    alloc_count++;
    total_bytes += size;
}

/* ---- Internal: remove a record by address ---- */
static int record_free(void *addr, size_t *out_size)
{
    AllocRecord **pp = &alloc_list;
    while (*pp) {
        if ((*pp)->addr == addr) {
            AllocRecord *found = *pp;
            *out_size = found->size;
            *pp = found->next;
            free_count++;
            total_bytes -= found->size;
            __real_free(found);
            return 1;
        }
        pp = &(*pp)->next;
    }
    return 0;
}

/* ---- Leak report (called at exit) ---- */
static void report_leaks(void)
{
    fprintf(stderr, "\n========================================\n");
    fprintf(stderr, "=== LEAK REPORT (malloc/free wrapper) ===\n");
    fprintf(stderr, "========================================\n");

    if (!alloc_list) {
        fprintf(stderr, "  No memory leaks detected.\n");
    } else {
        size_t leak_count = 0;
        size_t leak_bytes = 0;
        AllocRecord *cur = alloc_list;
        while (cur) {
            fprintf(stderr, "  Unfreed block: addr=%p, size=%zu bytes\n",
                    cur->addr, cur->size);
            leak_count++;
            leak_bytes += cur->size;
            cur = cur->next;
        }
        fprintf(stderr, "  --------------------------------\n");
        fprintf(stderr, "  Total leaked: %zu block(s), %zu bytes\n",
                leak_count, leak_bytes);
    }

    fprintf(stderr, "\nSummary: %zu allocation(s), %zu free(s)\n",
            alloc_count, free_count);
    fprintf(stderr, "========================================\n");
}

/* ---- Wrapper: intercepts all malloc() calls ---- */
void *__wrap_malloc(size_t size)
{
    void *ptr = __real_malloc(size);
    if (ptr)
        record_alloc(ptr, size);

    fprintf(stderr, "[ALLOC] malloc(%zu) = %p\n", size, ptr);
    return ptr;
}

/* ---- Wrapper: intercepts all free() calls ---- */
void __wrap_free(void *ptr)
{
    if (!ptr) {
        fprintf(stderr, "[FREE]  free(NULL) -- ignored\n");
        __real_free(ptr);
        return;
    }

    size_t freed_size = 0;
    if (record_free(ptr, &freed_size)) {
        fprintf(stderr, "[FREE]  free(%p) size=%zu\n", ptr, freed_size);
    } else {
        fprintf(stderr, "[FREE]  free(%p) WARNING: address not in allocation log!\n", ptr);
    }

    __real_free(ptr);
}

/* ---- Test program with intentional leaks ---- */
int main(void)
{
    atexit(report_leaks);

    printf("=== Variant 15: malloc/free wrapper with leak detection ===\n\n");

    /* 1. Normal allocation + free (no leak) */
    printf("Test 1: Normal malloc + free\n");
    char *a = malloc(100);
    if (a) {
        strcpy(a, "Hello, world!");
        printf("  a = \"%s\"\n", a);
    }
    free(a);

    /* 2. Intentional leak: allocated, never freed */
    printf("\nTest 2: Intentional leak (800 bytes, never freed)\n");
    int *b = malloc(200 * sizeof(int));
    if (b)
        b[0] = 42;

    /* 3. Pointer overwritten -> first block leaked */
    printf("\nTest 3: Pointer overwritten (50-byte block leaked)\n");
    char *c = malloc(50);
    if (c)
        c[0] = 'X';
    c = malloc(75);  /* 50-byte block is now leaked */
    free(c);         /* only 75-byte block freed */

    /* 4. Normal allocation + free (no leak) */
    printf("\nTest 4: Another normal malloc + free\n");
    double *d = malloc(10 * sizeof(double));
    if (d)
        d[0] = 3.14;
    free(d);

    /* 5. Intentional leak: allocated in scope, never freed */
    printf("\nTest 5: Intentional leak (128 bytes, never freed)\n");
    void *e = malloc(128);
    (void)e;  /* suppress unused warning */

    /* 6. Multiple small allocations, free only some */
    printf("\nTest 6: 5 allocations, only 3 freed\n");
    void *arr[5];
    for (int i = 0; i < 5; i++)
        arr[i] = malloc(32);
    free(arr[0]);
    free(arr[2]);
    free(arr[4]);
    /* arr[1] and arr[3] are leaked */

    printf("\nProgram ending -- atexit leak report follows on stderr:\n");
    return 0;
}
