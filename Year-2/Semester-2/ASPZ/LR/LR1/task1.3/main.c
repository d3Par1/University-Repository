/*
 * Task 1.3 - main.c
 * Multi-file program demonstrating library linking
 */
#include <stdio.h>
#include "process1.h"

/* Forward declarations from other modules */
void read_input(void);
void write_output(void);

int main(int argc, char *argv[]) {
    printf("=== Task 1.3: Multi-file Compilation Demo ===\n\n");

    read_input();
    process1_run();
    process_run();
    write_output();

    printf("\nAll modules executed successfully.\n");
    return 0;
}
