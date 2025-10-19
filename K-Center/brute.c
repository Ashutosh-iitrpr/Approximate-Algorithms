#include "brute.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

typedef struct {
    int** dist_matrix;
    int n;
    int k;
    int min_radius;
    int* best_centers;
} BruteForceContext;

int calculate_radius(int* centers, BruteForceContext* ctx) {
    int max_dist = 0;
    for (int i = 0; i < ctx->n; i++) {
        int min_dist_to_center = INT_MAX;
        for (int j = 0; j < ctx->k; j++) {
            int center_idx = centers[j];
            if (ctx->dist_matrix[i][center_idx] < min_dist_to_center) {
                min_dist_to_center = ctx->dist_matrix[i][center_idx];
            }
        }
        if (min_dist_to_center > max_dist) max_dist = min_dist_to_center;
    }
    return max_dist;
}

void find_combinations_recursive(int start_index, int count, int* current_combination, BruteForceContext* ctx) {
    if (count == ctx->k) {
        int radius = calculate_radius(current_combination, ctx);
        if (radius < ctx->min_radius) {
            ctx->min_radius = radius;
            for (int i = 0; i < ctx->k; i++) {
                ctx->best_centers[i] = current_combination[i];
            }
        }
        return;
    }
    // Optimization: Stop if we can't possibly form a full k-combination
    if (start_index >= ctx->n) return;

    for (int i = start_index; i <= ctx->n - (ctx->k - count); i++) {
        current_combination[count] = i;
        find_combinations_recursive(i + 1, count + 1, current_combination, ctx);
    }
}

int brute_force_k_center(int n, int k, int** dist_matrix) {
    printf("--- Running Brute-Force (Exact) Algorithm ---\n");
    clock_t start = clock();

    BruteForceContext ctx;
    ctx.dist_matrix = dist_matrix;
    ctx.n = n;
    ctx.k = k;
    ctx.min_radius = INT_MAX;
    ctx.best_centers = (int*)malloc(k * sizeof(int));

    int* current_combination = (int*)malloc(k * sizeof(int));
    
    find_combinations_recursive(0, 0, current_combination, &ctx);
    
    clock_t end = clock();
    double time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("Result:\n");
    printf("  Execution Time: %f seconds\n", time_used);
    printf("  Optimal Radius: %d\n", ctx.min_radius);
    printf("  Centers: { ");
    for (int i = 0; i < k; i++) {
        printf("%d%s", ctx.best_centers[i], (i == k - 1) ? "" : ", ");
    }
    printf(" }\n\n");

    free(ctx.best_centers);
    free(current_combination);
    return ctx.min_radius;
}