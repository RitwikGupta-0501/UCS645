#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int N = 1 << 16; // 2^16 elements
    int local_N = N / size;
    double a = 2.5; // Scalar multiplier

    double *X = NULL, *Y = NULL;
    double *local_X = (double*)malloc(local_N * sizeof(double));
    double *local_Y = (double*)malloc(local_N * sizeof(double));

    if (rank == 0) {
        X = (double*)malloc(N * sizeof(double));
        Y = (double*)malloc(N * sizeof(double));
        for (int i = 0; i < N; i++) {
            X[i] = 1.0;
            Y[i] = 2.0;
        }
    }

    double start_time = MPI_Wtime();

    // Distribute data
    MPI_Scatter(X, local_N, MPI_DOUBLE, local_X, local_N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(Y, local_N, MPI_DOUBLE, local_Y, local_N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // DAXPY Computation
    for (int i = 0; i < local_N; i++) {
        local_X[i] = a * local_X[i] + local_Y[i];
    }

    // Gather results
    MPI_Gather(local_X, local_N, MPI_DOUBLE, X, local_N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    double end_time = MPI_Wtime();

    if (rank == 0) {
        printf("DAXPY Computation Complete.\n");
        printf("Total Time (Processes = %d): %f seconds\n", size, end_time - start_time);
        free(X);
        free(Y);
    }

    free(local_X);
    free(local_Y);
    MPI_Finalize();
    return 0;
}
