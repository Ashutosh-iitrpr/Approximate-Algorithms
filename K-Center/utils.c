#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int** allocate_matrix(int n) {
    int** matrix = (int**)malloc(n * sizeof(int*));
    for (int i = 0; i < n; i++) {
        matrix[i] = (int*)calloc(n, sizeof(int));
    }
    return matrix;
}

void free_matrix(int n, int** matrix) {
    if (matrix == NULL) return;
    for (int i = 0; i < n; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

int** generate_random_metric_data(int n) {
    int** dist_matrix = allocate_matrix(n);
    int* x_coords = (int*)malloc(n * sizeof(int));
    int* y_coords = (int*)malloc(n * sizeof(int));

    printf("Generating a %d x %d distance matrix from random 2D points...\n\n", n, n);
    for (int i = 0; i < n; i++) {
        x_coords[i] = rand() % 1000; // Coords between 0 and 999
        y_coords[i] = rand() % 1000;
    }

    for (int i = 0; i < n; i++) {
        for (int j = i; j < n; j++) {
            if (i == j) {
                dist_matrix[i][j] = 0;
            } else {
                double dist = sqrt(pow(x_coords[i] - x_coords[j], 2) + pow(y_coords[i] - y_coords[j], 2));
                dist_matrix[i][j] = (int)round(dist);
                dist_matrix[j][i] = (int)round(dist);
            }
        }
    }

    free(x_coords);
    free(y_coords);
    return dist_matrix;
}