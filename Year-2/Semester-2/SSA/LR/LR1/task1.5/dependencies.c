// Завдання 1.5: Демонстрація залежностей
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
