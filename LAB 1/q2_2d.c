#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define SIZE 1000

int main() {

    double **A = malloc(SIZE * sizeof(double*));
    double **B = malloc(SIZE * sizeof(double*));
    double **C = malloc(SIZE * sizeof(double*));
    for (int i = 0; i < SIZE; i++) {
        A[i] = malloc(SIZE * sizeof(double));
        B[i] = malloc(SIZE * sizeof(double));
        C[i] = malloc(SIZE * sizeof(double));
    }

    // Initialization
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            A[i][j] = 1.0; B[i][j] = 2.0;
        }
    }

    double start = omp_get_wtime();
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            double sum = 0;
            for (int k = 0; k < SIZE; k++) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
    double end = omp_get_wtime();

    printf("%f\n", end - start);
    return 0;
}
