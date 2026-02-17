// Завдання 1.12: Узагальнений масив
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef enum { TYPE_INT, TYPE_FLOAT, TYPE_CHAR } DataType;

void fill_random(void *arr, int size, DataType type) {
    for (int i = 0; i < size; i++) {
        switch (type) {
            case TYPE_INT:   *((int *)arr + i) = rand() % 100; break;
            case TYPE_FLOAT: *((float *)arr + i) = (float)(rand() % 10000) / 100.0f; break;
            case TYPE_CHAR:  *((char *)arr + i) = 'A' + (rand() % 26); break;
        }
    }
}

int recursive_search(const void *arr, int size, const void *target, DataType type, int idx) {
    if (idx >= size) return -1;

    int found = 0;
    switch (type) {
        case TYPE_INT:   found = (*((int *)arr + idx) == *(int *)target); break;
        case TYPE_FLOAT: found = (*((float *)arr + idx) == *(float *)target); break;
        case TYPE_CHAR:  found = (*((char *)arr + idx) == *(char *)target); break;
    }

    if (found) return idx;
    return recursive_search(arr, size, target, type, idx + 1);
}

int cmp_int(const void *a, const void *b) { return *(int *)a - *(int *)b; }
int cmp_float(const void *a, const void *b) {
    float fa = *(float *)a, fb = *(float *)b;
    return (fa > fb) - (fa < fb);
}
int cmp_char(const void *a, const void *b) { return *(char *)a - *(char *)b; }

int binary_search(const void *arr, int size, const void *target, DataType type) {
    size_t elem_size = 0;
    int (*cmp)(const void *, const void *) = NULL;

    switch (type) {
        case TYPE_INT:   elem_size = sizeof(int);   cmp = cmp_int;   break;
        case TYPE_FLOAT: elem_size = sizeof(float); cmp = cmp_float; break;
        case TYPE_CHAR:  elem_size = sizeof(char);  cmp = cmp_char;  break;
        default: return -1;
    }

    void *result = bsearch(target, arr, size, elem_size, cmp);
    if (result == NULL) return -1;
    return (int)((char *)result - (char *)arr) / (int)elem_size;
}

void print_array(const void *arr, int size, DataType type) {
    for (int i = 0; i < size; i++) {
        switch (type) {
            case TYPE_INT:   printf("%d ", *((int *)arr + i)); break;
            case TYPE_FLOAT: printf("%.2f ", *((float *)arr + i)); break;
            case TYPE_CHAR:  printf("%c ", *((char *)arr + i)); break;
        }
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    DataType type = TYPE_INT;
    if (argc > 1) {
        if (strcmp(argv[1], "float") == 0) type = TYPE_FLOAT;
        else if (strcmp(argv[1], "char") == 0) type = TYPE_CHAR;
    }

    srand(time(NULL));
    int size = 15;

    printf("=== Task 1.12: Generic Array (type=%s) ===\n\n",
           type == TYPE_INT ? "int" : type == TYPE_FLOAT ? "float" : "char");

    size_t elem_size = (type == TYPE_INT) ? sizeof(int) :
                       (type == TYPE_FLOAT) ? sizeof(float) : sizeof(char);
    void *arr = malloc(size * elem_size);
    fill_random(arr, size, type);

    printf("Array: ");
    print_array(arr, size, type);

    void *target = malloc(elem_size);
    memcpy(target, (char *)arr + (size / 2) * elem_size, elem_size);

    int idx = recursive_search(arr, size, target, type, 0);
    printf("Recursive search: found at index %d\n", idx);

    int (*cmp)(const void *, const void *) =
        (type == TYPE_INT) ? cmp_int : (type == TYPE_FLOAT) ? cmp_float : cmp_char;
    qsort(arr, size, elem_size, cmp);

    printf("Sorted:  ");
    print_array(arr, size, type);

    idx = binary_search(arr, size, target, type);
    printf("Binary search: found at index %d\n", idx);

    free(arr);
    free(target);
    return 0;
}
