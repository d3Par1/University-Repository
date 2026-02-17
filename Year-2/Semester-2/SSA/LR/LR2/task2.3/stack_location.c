/*
 * Task 2.3 - Stack, heap, data, text segment locations
 *
 * Compile: gcc -Wall -o task2_3 stack_location.c
 */
#include <stdio.h>
#include <stdlib.h>

/* Data segment (initialized) */
int global_init = 42;
/* BSS segment (uninitialized) */
int global_uninit;
/* Text segment */
const char *const_str = "constant string";

void deep_function(int depth) {
    int big_local[1024]; /* 4KB on stack per call */
    big_local[0] = depth;
    printf("  Depth %d: stack top near %p (diff from main: ~%d bytes)\n",
           depth, (void *)&big_local[0], 0);

    if (depth < 5) deep_function(depth + 1);
}

int main() {
    int stack_var;
    int *heap_ptr = malloc(sizeof(int));

    printf("=== Task 2.3: Memory Segments ===\n\n");

    printf("--- Segment Locations ---\n");
    printf("TEXT (code):      main = %p\n", (void *)main);
    printf("DATA (init):      global_init = %p (value=%d)\n",
           (void *)&global_init, global_init);
    printf("BSS  (uninit):    global_uninit = %p (value=%d)\n",
           (void *)&global_uninit, global_uninit);
    printf("HEAP (malloc):    heap_ptr = %p\n", (void *)heap_ptr);
    printf("STACK (local):    stack_var = %p\n", (void *)&stack_var);
    printf("CONST (rodata):   const_str = %p\n", (void *)const_str);

    printf("\n--- Stack Growth (deep calls with large locals) ---\n");
    printf("Stack top (main): %p\n", (void *)&stack_var);
    deep_function(1);

    /* Heap growth */
    printf("\n--- Heap Growth ---\n");
    for (int i = 0; i < 5; i++) {
        void *p = malloc(1024 * 1024); /* 1MB each */
        printf("  Alloc %d: %p\n", i, p);
        /* intentionally not freeing to show growth */
    }

    free(heap_ptr);
    printf("\nNote: On most architectures, stack grows DOWN, heap grows UP\n");

    return 0;
}
