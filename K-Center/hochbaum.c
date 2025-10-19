#include "hochbaum.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <limits.h>

// Note: main() and shared helpers removed.
// The helpers specific to this algorithm remain here.

// Helper for qsort
int compare_ints(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

// Gets unique, sorted costs from the distance matrix
int* get_unique_costs(int n, int** dist_matrix, int* m) {
    int max_costs = (n * (n - 1)) / 2;
    if (max_costs <= 0) {
        *m = 0;
        return NULL;
    }
    int* all_costs = (int*)malloc(max_costs * sizeof(int));
    int count = 0;
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            all_costs[count++] = dist_matrix[i][j];
        }
    }
    qsort(all_costs, count, sizeof(int), compare_ints);

    if (count == 0) { *m = 0; free(all_costs); return NULL; }

    int* unique_costs = (int*)malloc(count * sizeof(int));
    unique_costs[0] = all_costs[0];
    int unique_count = 1;
    for (int i = 1; i < count; i++) {
        if (all_costs[i] > all_costs[i-1]) {
            unique_costs[unique_count++] = all_costs[i];
        }
    }
    *m = unique_count;
    free(all_costs);
    return unique_costs;
}

// Finds a maximal independent set
int* find_maximal_independent_set(int n, bool** adj_matrix, int* mis_size) {
    int* mis = (int*)malloc(n * sizeof(int));
    *mis_size = 0;
    bool* covered = (bool*)calloc(n, sizeof(bool));

    for (int i = 0; i < n; i++) {
        if (!covered[i]) {
            mis[(*mis_size)++] = i;
            covered[i] = true;
            for (int j = 0; j < n; j++) {
                if (adj_matrix[i][j]) {
                    covered[j] = true;
                }
            }
        }
    }
    free(covered);
    return mis;
}

int hochbaum_shmoys_k_center(int n, int k, int** dist_matrix) {
    printf("--- Running Hochbaum & Shmoys Algorithm ---\n");
    clock_t start = clock();
    
    int m = 0;
    int* unique_costs = get_unique_costs(n, dist_matrix, &m);
    if (m == 0) { printf("No edges in graph.\n\n"); return -1; }

    int* final_centers = NULL;
    int final_mis_size = 0;
    int final_radius = -1;

    for (int j = 0; j < m; j++) {
        int r = unique_costs[j];
        
        // Build G_i^2 directly
        bool** g_sq = (bool**)malloc(n * sizeof(bool*));
        for(int i=0; i<n; ++i) g_sq[i] = (bool*)calloc(n, sizeof(bool));

        for (int u = 0; u < n; u++) {
            for (int v = u; v < n; v++) {
                if (dist_matrix[u][v] <= r) {
                    g_sq[u][v] = g_sq[v][u] = true;
                } else {
                    for (int w = 0; w < n; w++) {
                        if (dist_matrix[u][w] <= r && dist_matrix[w][v] <= r) {
                            g_sq[u][v] = g_sq[v][u] = true;
                            break;
                        }
                    }
                }
            }
        }
        
        int mis_size = 0;
        int* mis = find_maximal_independent_set(n, g_sq, &mis_size);
        for(int i=0; i<n; ++i) free(g_sq[i]);
        free(g_sq);

        if (mis_size <= k) {
            final_centers = mis;
            final_mis_size = mis_size;
            final_radius = r;
            break; 
        }
        free(mis);
    }
    
    int actual_radius = 0;
    if (final_centers != NULL) {
        for (int i = 0; i < n; i++) {
            int min_dist = INT_MAX;
            for (int c = 0; c < final_mis_size; c++) {
                if (dist_matrix[i][final_centers[c]] < min_dist) {
                    min_dist = dist_matrix[i][final_centers[c]];
                }
            }
            if (min_dist > actual_radius) actual_radius = min_dist;
        }
    }

    clock_t end = clock();
    double time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("Result:\n");
    printf("  Execution Time: %f seconds\n", time_used);
    printf("  Radius (2-approximation): %d\n", actual_radius);
    if(final_centers){
        printf("  Centers: { ");
        for (int i = 0; i < final_mis_size; i++) {
            printf("%d%s", final_centers[i], (i == final_mis_size - 1) ? "" : ", ");
        }
        printf(" }\n\n");
        free(final_centers);
    } else {
        printf("  No solution found.\n\n");
    }
    
    free(unique_costs);
    return actual_radius;
}