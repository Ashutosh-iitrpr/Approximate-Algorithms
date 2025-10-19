#include "gonzalez.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <limits.h>

int gonzalez_k_center(int n, int k, int** dist_matrix) {
    printf("--- Running Gonzalez's Farthest-First Algorithm ---\n");
    clock_t start = clock();

    int* min_dists = (int*)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) min_dists[i] = INT_MAX;

    int* centers = (int*)malloc(k * sizeof(int));
    bool* is_center = (bool*)calloc(n, sizeof(bool));
    
    centers[0] = 0; // Start with vertex 0
    is_center[0] = true;

    for (int i = 1; i < k; i++) {
        int last_center = centers[i - 1];
        for (int j = 0; j < n; j++) {
            if (dist_matrix[j][last_center] < min_dists[j]) {
                min_dists[j] = dist_matrix[j][last_center];
            }
        }
        
        int max_dist = -1;
        int farthest_vertex = -1;
        for (int j = 0; j < n; j++) {
            if (!is_center[j] && min_dists[j] > max_dist) {
                max_dist = min_dists[j];
                farthest_vertex = j;
            }
        }
        if (farthest_vertex != -1) {
            centers[i] = farthest_vertex;
            is_center[farthest_vertex] = true;
        }
    }
    
    int final_radius = 0;
    int last_added_center = centers[k-1];
    for(int i=0; i<n; ++i){
        if (dist_matrix[i][last_added_center] < min_dists[i]) {
            min_dists[i] = dist_matrix[i][last_added_center];
        }
        if(min_dists[i] > final_radius) {
            final_radius = min_dists[i];
        }
    }

    clock_t end = clock();
    double time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("Result:\n");
    printf("  Execution Time: %f seconds\n", time_used);
    printf("  Radius (2-approximation): %d\n", final_radius);
    printf("  Centers: { ");
    for (int i = 0; i < k; i++) {
        printf("%d%s", centers[i], (i == k - 1) ? "" : ", ");
    }
    printf(" }\n\n");

    free(min_dists);
    free(centers);
    free(is_center);
    return final_radius;
}