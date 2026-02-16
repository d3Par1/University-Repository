/*
 * Task 5.8: Use-after-free demonstration
 *
 * Accessing memory after it has been freed is undefined behavior.
 * The freed block may be returned by a subsequent malloc, so
 * writing through the old pointer silently corrupts new data.
 *
 * Compile: gcc -Wall -Wextra -std=c11 -g use_after_free.c -o use_after_free
 * Run:     ./use_after_free
 * Valgrind: valgrind ./use_after_free
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Demo 1: Read after free — may return old value */
static void demo_read_uaf(void) {
    printf("--- Demo 1: Read after free ---\n");
    int *p = malloc(sizeof(int));
    if (!p) return;
    *p = 42;
    printf("  Before free: *p = %d\n", *p);

    free(p);
    /* p is dangling — reading is UB */
    printf("  After free: *p may still appear as 42\n");
    printf("  But the memory may be reused by next malloc.\n");
    printf("  Valgrind: Invalid read of size 4\n\n");
}

/* Demo 2: Freed block reused by new allocation */
static void demo_reuse_uaf(void) {
    printf("--- Demo 2: Freed block reused ---\n");
    char *first = malloc(32);
    if (!first) return;
    strcpy(first, "ORIGINAL");
    printf("  first = \"%s\" at %p\n", first, (void *)first);

    free(first);

    /* New allocation may get the same block */
    char *second = malloc(32);
    if (!second) return;
    strcpy(second, "REPLACED");
    printf("  second = \"%s\" at %p\n", second, (void *)second);

    if (first == second) {
        printf("  first == second! Same address reused.\n");
        printf("  Writing through 'first' would corrupt 'second'.\n");
    } else {
        printf("  Different addresses (allocator chose a different block).\n");
    }
    free(second);
    printf("\n");
}

/* Demo 3: Use-after-free in library-like code */
typedef struct {
    int id;
    char name[32];
} Record;

static Record *create_record(int id, const char *name) {
    Record *r = malloc(sizeof(Record));
    if (!r) return NULL;
    r->id = id;
    strncpy(r->name, name, sizeof(r->name) - 1);
    r->name[sizeof(r->name) - 1] = '\0';
    return r;
}

static void demo_struct_uaf(void) {
    printf("--- Demo 3: Struct use-after-free ---\n");
    Record *r = create_record(1, "test_user");
    printf("  Created: id=%d, name=\"%s\"\n", r->id, r->name);

    free(r);
    printf("  Record freed.\n");

    /*
     * Accessing r->id or r->name after free is UB.
     * If another malloc reuses this block, data is corrupted.
     */
    printf("  r->id after free: accessing would be UB\n");
    printf("  Valgrind: Invalid read inside a block of size %zu free'd\n\n",
           sizeof(Record));
}

int main(void) {
    printf("=== Task 5.8: Use-After-Free ===\n\n");

    printf("Use-after-free: accessing memory after free().\n");
    printf("The freed block may be recycled by the allocator,\n");
    printf("so old pointers silently read/write wrong data.\n\n");

    demo_read_uaf();
    demo_reuse_uaf();
    demo_struct_uaf();

    printf("=== Summary ===\n");
    printf("Use-after-free is a top security vulnerability (CWE-416).\n");
    printf("Prevention:\n");
    printf("  1. Set ptr = NULL after free\n");
    printf("  2. Clear ownership semantics\n");
    printf("  3. Detect with valgrind or -fsanitize=address\n");

    return 0;
}
