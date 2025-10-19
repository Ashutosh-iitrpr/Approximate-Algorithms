#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils.h"
#include "gonzalez.h"
#include "hochbaum.h"
#include "brute.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <number_of_vertices_n> <number_of_centers_k>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    int k = atoi(argv[2]);

    if (n <= 0 || k <= 0 || k > n) {
        fprintf(stderr, "Error: Invalid input. Ensure n > 0, k > 0, and k <= n.\n");
        return 1;
    }
    
    srand(time(NULL)); 

    printf("====================================================\n");
    printf("     K-Center Algorithm Comparison\n");
    printf("====================================================\n");
    printf("Test case parameters:\n");
    printf("  Number of vertices (n): %d\n", n);
    printf("  Number of centers (k): %d\n\n", k);

    // Generate the test data
    int** dist_matrix = generate_random_metric_data(n);
    //print it
    printf("Generated Distance Matrix:\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%4d ", dist_matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    int rad1 = gonzalez_k_center(n, k, dist_matrix);
    int rad2 = hochbaum_shmoys_k_center(n, k, dist_matrix);
    
    // Warning for the brute force algorithm
    // if (n > 25) {
    //      printf("--- Skipping Brute-Force (Exact) Algorithm ---\n");
    //      printf("n=%d is too large for an exact solution in a reasonable time.\n\n", n);
    // } else {
        int rad3 = brute_force_k_center(n, k, dist_matrix);       
        printf("====================================================\n");
        printf("%% change of the Radii from Brute Force:\n");
        if (rad3 != -1) {
            double var1 = ((double)(rad1 - rad3) / rad3) * 100.0;
            double var2 = ((double)(rad2 - rad3) / rad3) * 100.0;
            printf("  Gonzalez's Algorithm: %.2f%%\n", var1);
            printf("  Hochbaum & Shmoys Algorithm: %.2f%%\n", var2);
        }
    

    
    
    printf("====================================================\n");
    printf("Comparison complete.\n");
    printf("====================================================\n");

    free_matrix(n, dist_matrix);

    return 0;
}