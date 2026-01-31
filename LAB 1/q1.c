#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define VECTOR_SIZE 65536 

int main() {
    double *X = (double*)malloc(VECTOR_SIZE * sizeof(double));
    double *Y = (double*)malloc(VECTOR_SIZE * sizeof(double));
    double a = 2.5;
    double start, end;

    if (X == NULL || Y == NULL) return 1;

    // Initialization
    for (int i = 0; i < VECTOR_SIZE; i++) {
        X[i] = i * 0.5;
        Y[i] = i * 1.5;
    }

    // Parallel DAXPY Loop
    start = omp_get_wtime();
    #pragma omp parallel for
    for (int i = 0; i < VECTOR_SIZE; i++) {
        X[i] = a * X[i] + Y[i];
    }
    end = omp_get_wtime();

    // Printing the execution time
    printf("%f\n", end - start);

    free(X);
    free(Y);
    return 0;
}
