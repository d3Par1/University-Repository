/*
 * Task 2.2 - Segment analysis source (fruits example from lecture)
 *
 * Compile: gcc -Wall fruts.c -o fruts
 * Analyze: nm fruts | sort; size fruts; objdump -t fruts
 */
#include <stdio.h>
#include <stdlib.h>

char pear[40];             /* BSS (uninitialized global) */
static double peach;       /* BSS (uninitialized static) */
int mango = 13;            /* DATA (initialized global) */
static long melon = 2001;  /* DATA (initialized static) */

int main() {
    int i = 3, j, *ip;
    ip = malloc(sizeof(i));  /* HEAP */
    pear[5] = i;
    peach = 2.0 * mango;

    printf("Segment locations:\n");
    printf("  pear  (BSS global):  %p\n", (void *)pear);
    printf("  peach (BSS static):  %p\n", (void *)&peach);
    printf("  mango (DATA global): %p\n", (void *)&mango);
    printf("  melon (DATA static): %p\n", (void *)&melon);
    printf("  i     (STACK local): %p\n", (void *)&i);
    printf("  ip    (HEAP alloc):  %p\n", (void *)ip);
    printf("  main  (TEXT code):   %p\n", (void *)main);

    free(ip);
    (void)j; /* suppress warning */
    return 0;
}
