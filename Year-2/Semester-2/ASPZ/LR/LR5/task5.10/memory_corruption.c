/*
 * Task 5.10: Memory corruption
 *
 * Writing beyond one data structure's bounds can silently modify
 * adjacent data in memory. This is one of the hardest bugs to debug
 * because the corruption manifests far from the actual error.
 *
 * Compile: gcc -Wall -Wextra -std=c11 -g memory_corruption.c -o memory_corruption
 * Run:     ./memory_corruption
 * Valgrind: valgrind ./memory_corruption
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Stack corruption: array overwrite modifies adjacent variable */
static void demo_stack_corruption(void) {
    printf("--- Stack memory corruption ---\n");

    int b = 5;
    int a[3] = {1, 2, 3};

    printf("  Before: a = {%d, %d, %d}, b = %d\n", a[0], a[1], a[2], b);

    /* Out-of-bounds write — may overwrite b */
    a[5] = 99;

    printf("  After a[5]=99: a = {%d, %d, %d}, b = %d\n",
           a[0], a[1], a[2], b);
    if (b != 5)
        printf("  b was CORRUPTED by writing past a[]!\n");
    else
        printf("  b unchanged (compiler may have placed b elsewhere)\n");
    printf("\n");
}

/* Heap corruption: overwriting between two structures */
static void demo_heap_corruption(void) {
    printf("--- Heap memory corruption ---\n");

    typedef struct {
        char name[8];
        int value;
    } Record;

    Record *r1 = malloc(sizeof(Record));
    Record *r2 = malloc(sizeof(Record));
    if (!r1 || !r2) return;

    strcpy(r1->name, "Alpha");
    r1->value = 100;
    strcpy(r2->name, "Beta");
    r2->value = 200;

    printf("  Before: r1 = {name=\"%s\", value=%d}\n", r1->name, r1->value);
    printf("  Before: r2 = {name=\"%s\", value=%d}\n", r2->name, r2->value);

    /* Overflow r1->name — may corrupt r1->value or heap metadata */
    strcpy(r1->name, "AAAAAAAAAAAAAAAA"); /* 16 chars into 8-byte buffer */

    printf("  After overflow:\n");
    printf("  r1 = {name=\"%s\", value=%d}\n", r1->name, r1->value);
    printf("  r2 = {name=\"%s\", value=%d}\n", r2->name, r2->value);

    if (r1->value != 100)
        printf("  r1->value CORRUPTED by name overflow!\n");
    printf("\n");

    free(r1);
    free(r2);
}

/* Corruption via wrong memcpy size */
static void demo_memcpy_corruption(void) {
    printf("--- memcpy corruption ---\n");

    int src[4] = {10, 20, 30, 40};
    int dst[2]; /* only space for 2 ints */
    int guard = 0xBEEF;

    printf("  Before: guard = 0x%X\n", guard);

    /* Copy 4 ints into 2-int buffer — overflows */
    memcpy(dst, src, sizeof(src)); /* 16 bytes into 8-byte buffer */

    printf("  After: dst = {%d, %d}, guard = 0x%X",
           dst[0], dst[1], guard);
    if (guard != 0xBEEF)
        printf(" (CORRUPTED!)");
    printf("\n\n");
}

int main(void) {
    printf("=== Task 5.10: Memory Corruption ===\n\n");

    demo_stack_corruption();
    demo_heap_corruption();
    demo_memcpy_corruption();

    printf("=== Summary ===\n");
    printf("Memory corruption silently damages unrelated data.\n");
    printf("The bug and the symptom appear in different places.\n");
    printf("Tools: valgrind, ASan (-fsanitize=address), magic canaries.\n");

    return 0;
}
