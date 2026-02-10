// Завдання 1.13: Динамічний масив
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN_SIZE 1
#define MAX_SIZE 1000000

static size_t total_allocated = 0;
static int malloc_count = 0;
static int realloc_count = 0;
static int free_count = 0;

void *tracked_malloc(size_t size) {
    void *ptr = malloc(size);
    if (ptr) {
        total_allocated += size;
        malloc_count++;
        printf("[mem] malloc(%zu) -> total=%zu bytes\n", size, total_allocated);
    }
    return ptr;
}

void *tracked_realloc(void *ptr, size_t old_size, size_t new_size) {
    void *new_ptr = realloc(ptr, new_size);
    if (new_ptr) {
        total_allocated = total_allocated - old_size + new_size;
        realloc_count++;
        printf("[mem] realloc(%zu -> %zu) -> total=%zu bytes\n",
               old_size, new_size, total_allocated);
    }
    return new_ptr;
}

void tracked_free(void *ptr, size_t size) {
    free(ptr);
    total_allocated -= size;
    free_count++;
    printf("[mem] free(%zu) -> total=%zu bytes\n", size, total_allocated);
}

void print_memory_stats(void) {
    printf("\n--- Memory Stats ---\n");
    printf("  malloc calls:  %d\n", malloc_count);
    printf("  realloc calls: %d\n", realloc_count);
    printf("  free calls:    %d\n", free_count);
    printf("  current usage: %zu bytes\n", total_allocated);
}

void print_array(const int *arr, int size) {
    printf("Array[%d]: ", size);
    int limit = (size > 20) ? 20 : size;
    for (int i = 0; i < limit; i++) printf("%d ", arr[i]);
    if (size > 20) printf("...");
    printf("\n");
}

int main() {
    int n;
    printf("=== Task 1.13: Dynamic Array ===\n\n");

    printf("Enter array size (%d-%d): ", MIN_SIZE, MAX_SIZE);
    if (scanf("%d", &n) != 1 || n < MIN_SIZE || n > MAX_SIZE) {
        fprintf(stderr, "Invalid size!\n");
        return 1;
    }

    size_t current_size = n * sizeof(int);
    int *arr = (int *)tracked_malloc(current_size);
    if (!arr) { fprintf(stderr, "Allocation failed!\n"); return 1; }

    for (int i = 0; i < n; i++) arr[i] = n;
    print_array(arr, n);

    int choice;
    while (1) {
        printf("\n1) Grow  2) Shrink  3) Print  4) Stats  0) Exit\n> ");
        if (scanf("%d", &choice) != 1) break;

        switch (choice) {
            case 1: {
                int new_n;
                printf("New size: ");
                scanf("%d", &new_n);
                if (new_n < MIN_SIZE || new_n > MAX_SIZE) {
                    printf("Invalid! (%d-%d)\n", MIN_SIZE, MAX_SIZE);
                    break;
                }
                size_t new_size = new_n * sizeof(int);
                int *new_arr = (int *)tracked_realloc(arr, current_size, new_size);
                if (!new_arr) { printf("Realloc failed!\n"); break; }
                for (int i = n; i < new_n; i++) new_arr[i] = 0;
                arr = new_arr;
                n = new_n;
                current_size = new_size;
                printf("Grown to %d elements.\n", n);
                break;
            }
            case 2: {
                int new_n;
                printf("New size: ");
                scanf("%d", &new_n);
                if (new_n < MIN_SIZE || new_n > MAX_SIZE || new_n >= n) {
                    printf("Invalid! Must be < %d\n", n);
                    break;
                }
                size_t new_size = new_n * sizeof(int);
                int *new_arr = (int *)tracked_realloc(arr, current_size, new_size);
                if (!new_arr) { printf("Realloc failed!\n"); break; }
                arr = new_arr;
                n = new_n;
                current_size = new_size;
                printf("Shrunk to %d elements.\n", n);
                break;
            }
            case 3: print_array(arr, n); break;
            case 4: print_memory_stats(); break;
            case 0:
                tracked_free(arr, current_size);
                print_memory_stats();
                return 0;
        }
    }

    tracked_free(arr, current_size);
    return 0;
}
