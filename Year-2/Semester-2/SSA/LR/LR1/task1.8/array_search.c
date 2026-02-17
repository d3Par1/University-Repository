// Завдання 1.8: Пошук у масиві
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define MAX_DIM 3
#define TOLERANCE 0.001

void fill_random_1d(float *arr, int size, float max_val) {
    for (int i = 0; i < size; i++) {
        arr[i] = ((float)rand() / RAND_MAX) * max_val;
    }
}

int find_all_1d(const float *arr, int size, float target, int *indices, int max_results) {
    int count = 0;
    for (int i = 0; i < size && count < max_results; i++) {
        if (fabsf(arr[i] - target) < TOLERANCE) {
            indices[count++] = i;
        }
    }
    return count;
}

void print_array_1d(const float *arr, int size) {
    for (int i = 0; i < size; i++) {
        printf("[%d]=%.3f ", i, arr[i]);
        if ((i + 1) % 5 == 0) printf("\n");
    }
    printf("\n");
}

void demo_2d(int rows, int cols, float max_val) {
    printf("\n--- 2D Array (%dx%d) ---\n", rows, cols);

    float **arr2d = malloc(rows * sizeof(float *));
    for (int i = 0; i < rows; i++) {
        arr2d[i] = malloc(cols * sizeof(float));
        fill_random_1d(arr2d[i], cols, max_val);
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%6.3f ", arr2d[i][j]);
        }
        printf("\n");
    }

    float target = arr2d[rows / 2][cols / 2];
    printf("Searching for %.3f (tolerance %.3f):\n", target, TOLERANCE);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (fabsf(arr2d[i][j] - target) < TOLERANCE) {
                printf("  Found at [%d][%d] = %.3f\n", i, j, arr2d[i][j]);
            }
        }
    }

    for (int i = 0; i < rows; i++) free(arr2d[i]);
    free(arr2d);
}

void demo_3d(int d1, int d2, int d3, float max_val) {
    printf("\n--- 3D Array (%dx%dx%d) ---\n", d1, d2, d3);

    float ***arr3d = malloc(d1 * sizeof(float **));
    for (int i = 0; i < d1; i++) {
        arr3d[i] = malloc(d2 * sizeof(float *));
        for (int j = 0; j < d2; j++) {
            arr3d[i][j] = malloc(d3 * sizeof(float));
            fill_random_1d(arr3d[i][j], d3, max_val);
        }
    }

    float target = arr3d[0][0][0];
    printf("Searching for %.3f in 3D array:\n", target);
    for (int i = 0; i < d1; i++)
        for (int j = 0; j < d2; j++)
            for (int k = 0; k < d3; k++)
                if (fabsf(arr3d[i][j][k] - target) < TOLERANCE)
                    printf("  Found at [%d][%d][%d] = %.3f\n", i, j, k, arr3d[i][j][k]);

    for (int i = 0; i < d1; i++) {
        for (int j = 0; j < d2; j++) free(arr3d[i][j]);
        free(arr3d[i]);
    }
    free(arr3d);
}

int main() {
    srand(time(NULL));

    int size = 20;
    float max_val = 10.0;

    printf("=== Task 1.8: Random Float Array Search ===\n\n");

    printf("--- 1D Array (size=%d) ---\n", size);
    float *arr = malloc(size * sizeof(float));
    fill_random_1d(arr, size, max_val);
    print_array_1d(arr, size);

    float target = arr[size / 3];
    int indices[100];
    int found = find_all_1d(arr, size, target, indices, 100);
    printf("Search for %.3f: found %d occurrence(s)\n", target, found);
    for (int i = 0; i < found; i++) {
        printf("  Index %d: %.3f\n", indices[i], arr[indices[i]]);
    }
    free(arr);

    demo_2d(4, 5, max_val);
    demo_3d(2, 3, 4, max_val);

    return 0;
}
