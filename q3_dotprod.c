#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    long total_elements = 500000000;
    long local_elements = total_elements / size;

    double multiplier = 0.0;

    double start_time = MPI_Wtime();

    // 1. Initialization & Broadcast
    if (rank == 0) {
        multiplier = 1.5; // Arbitrary scaling multiplier
    }
    MPI_Bcast(&multiplier, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // 2. Local Generation (Saving memory on Rank 0)
    double *local_A = (double*)malloc(local_elements * sizeof(double));
    double *local_B = (double*)malloc(local_elements * sizeof(double));

    for (long i = 0; i < local_elements; i++) {
        local_A[i] = 1.0;
        local_B[i] = 2.0 * multiplier;
    }

    // 3. Local Compute
    double local_dot = 0.0;
    for (long i = 0; i < local_elements; i++) {
        local_dot += local_A[i] * local_B[i];
    }

    // 4. Global Reduction
    double final_result = 0.0;
    MPI_Reduce(&local_dot, &final_result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    double end_time = MPI_Wtime();

    if (rank == 0) {
        printf("Distributed Dot Product Result: %f\n", final_result);
        printf("Total Time (Processes = %d): %f seconds\n", size, end_time - start_time);
    }

    free(local_A);
    free(local_B);
    MPI_Finalize();
    return 0;
}
