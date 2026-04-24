#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

void MyBcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm) {
    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    if (rank == root) {
        for (int i = 0; i < size; i++) {
            if (i != root) {
                MPI_Send(buffer, count, datatype, i, 0, comm);
            }
        }
    } else {
        MPI_Recv(buffer, count, datatype, root, 0, comm, MPI_STATUS_IGNORE);
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    long N = 10000000; // 10 million doubles (~80 MB)
    double *arr = (double*)malloc(N * sizeof(double));

    if (rank == 0) {
        for (long i = 0; i < N; i++) arr[i] = 1.0;
    }

    // Synchronize before timing
    MPI_Barrier(MPI_COMM_WORLD);

    // --- Part A: MyBcast (Linear) ---
    double start_mybcast = MPI_Wtime();
    MyBcast(arr, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    double end_mybcast = MPI_Wtime();

    if (rank == 0) {
        printf("MyBcast (Linear) Time for %d processes: %f seconds\n", size, end_mybcast - start_mybcast);
    }

    // Reset array state conceptually
    MPI_Barrier(MPI_COMM_WORLD);

    // --- Part B: MPI_Bcast (Tree) ---
    double start_mpibcast = MPI_Wtime();
    MPI_Bcast(arr, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    double end_mpibcast = MPI_Wtime();

    if (rank == 0) {
        printf("MPI_Bcast (Tree) Time for %d processes: %f seconds\n", size, end_mpibcast - start_mpibcast);
    }

    free(arr);
    MPI_Finalize();
    return 0;
}
