/*
 * Task 5.5: Heap memory fragmentation
 *
 * Demonstrates how interleaved malloc/free creates fragmentation:
 * free memory exists but is scattered in small non-contiguous chunks,
 * preventing large allocations even though total free memory is sufficient.
 *
 * Compile: gcc -Wall -Wextra -std=c11 -g fragmentation.c -o fragmentation
 * Run:     ./fragmentation
 */

#include <stdio.h>
#include <stdlib.h>

#define SMALL_COUNT 1000
#define SMALL_SIZE  64
#define LARGE_SIZE  (SMALL_COUNT * SMALL_SIZE / 2)  /* 32 KB */

int main(void) {
    printf("=== Task 5.5: Heap Memory Fragmentation ===\n\n");

    /* Step 1: Allocate many small blocks */
    printf("--- Step 1: Allocating %d small blocks of %d bytes ---\n",
           SMALL_COUNT, SMALL_SIZE);

    void *blocks[SMALL_COUNT];
    for (int i = 0; i < SMALL_COUNT; i++) {
        blocks[i] = malloc(SMALL_SIZE);
        if (!blocks[i]) {
            printf("  malloc failed at block %d!\n", i);
            break;
        }
    }
    printf("  Allocated %d blocks = %d bytes total\n\n",
           SMALL_COUNT, SMALL_COUNT * SMALL_SIZE);

    /* Step 2: Free every other block — creates fragmented free list */
    printf("--- Step 2: Freeing every other block (creating gaps) ---\n");
    int freed = 0;
    for (int i = 0; i < SMALL_COUNT; i += 2) {
        free(blocks[i]);
        blocks[i] = NULL;
        freed++;
    }
    printf("  Freed %d blocks = %d bytes\n", freed, freed * SMALL_SIZE);
    printf("  Remaining: %d blocks still allocated (creating barriers)\n\n",
           SMALL_COUNT - freed);

    /*
     * Memory layout (simplified):
     *   [FREE][USED][FREE][USED][FREE][USED]...
     * Each free gap is only SMALL_SIZE bytes.
     * Total free = %d bytes, but largest contiguous = SMALL_SIZE.
     */
    printf("  Heap layout: [FREE %dB][USED %dB][FREE %dB][USED %dB]...\n",
           SMALL_SIZE, SMALL_SIZE, SMALL_SIZE, SMALL_SIZE);
    printf("  Total free: %d bytes, but largest contiguous block: %d bytes\n\n",
           freed * SMALL_SIZE, SMALL_SIZE);

    /* Step 3: Try to allocate a large block */
    printf("--- Step 3: Attempting to allocate %d bytes ---\n", LARGE_SIZE);
    void *large = malloc(LARGE_SIZE);
    if (large) {
        printf("  Success! (allocator may use mmap or coalesce internally)\n");
        printf("  Modern glibc is smart — it can use mmap for large blocks.\n");
        free(large);
    } else {
        printf("  FAILED! Fragmentation prevented allocation.\n");
    }
    printf("\n");

    /* Step 4: Demonstrate the principle with strict constraints */
    printf("--- Step 4: Fragmentation visualization ---\n");
    printf("  Before freeing:\n");
    printf("  [a:8B][b:128B][c:8B]\n");
    printf("\n");
    printf("  After free(b):\n");
    printf("  [a:8B][  F R E E  ][c:8B]\n");
    printf("  Free: 128B, but split by a and c on both sides.\n");
    printf("  Cannot allocate 256B contiguously!\n\n");

    /* Cleanup remaining blocks */
    for (int i = 1; i < SMALL_COUNT; i += 2)
        free(blocks[i]);

    printf("=== Summary ===\n");
    printf("Fragmentation = enough total free memory but not contiguous.\n");
    printf("Mitigation: memory pools, slab allocators, or jemalloc/tcmalloc.\n");

    return 0;
}
