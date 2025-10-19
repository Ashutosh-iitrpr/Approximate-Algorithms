#ifndef UTILS_H
#define UTILS_H

int** allocate_matrix(int n);
void free_matrix(int n, int** matrix);
int** generate_random_metric_data(int n);

#endif