/*
 * Task 1.5 - Dependency analysis (answers)
 *
 * File dependencies:
 *   main.c    -> stdio.h, process1.h
 *   input.c   -> stdio.h, list.h
 *   output.c  -> stdio.h
 *   process1.c -> stdio.h, process1.h
 *   process2.c -> stdio.h, list.h
 *   utility.c -> stdio.h, process1.h  (new file)
 *
 * Q1: After changing process1.c:
 *   -> Only process1.c needs recompilation (then re-link all)
 *
 * Q2: After changing process1.h:
 *   -> main.c, process1.c, utility.c need recompilation
 *   (all files that #include process1.h)
 *
 * Q3: After changing list.h:
 *   -> input.c, process2.c need recompilation
 *   (all files that #include list.h)
 *
 * Q4: Adding utility.c with deps on process1.h and stdio.h:
 *   -> utility.c depends on process1.h changes
 */

/* --- Demonstration files below --- */

/* list.h */
/* #ifndef LIST_H */
/* #define LIST_H */
/* void list_add(int item); */
/* int list_get(int index); */
/* #endif */

#include <stdio.h>

int main() {
    printf("=== Task 1.5: Dependency Analysis ===\n\n");

    printf("Files and their header dependencies:\n");
    printf("  main.c     -> stdio.h, process1.h\n");
    printf("  input.c    -> stdio.h, list.h\n");
    printf("  output.c   -> stdio.h\n");
    printf("  process1.c -> stdio.h, process1.h\n");
    printf("  process2.c -> stdio.h, list.h\n");
    printf("  utility.c  -> stdio.h, process1.h\n");

    printf("\nQ1: Changed process1.c -> recompile: process1.c\n");
    printf("Q2: Changed process1.h -> recompile: main.c, process1.c, utility.c\n");
    printf("Q3: Changed list.h     -> recompile: input.c, process2.c\n");

    return 0;
}
