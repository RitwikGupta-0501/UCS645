#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int value;
  int next_rank = (rank + 1) % size;
  int prev_rank = (rank - 1 + size) % size;

  // Synchronize all processes before timing
  MPI_Barrier(MPI_COMM_WORLD);
  double start_time = MPI_Wtime();

  if (rank == 0) {
    value = 100;
    printf("Process 0 starts with value: %d\n", value);

    value += rank;
    MPI_Send(&value, 1, MPI_INT, next_rank, 0, MPI_COMM_WORLD);

    MPI_Recv(&value, 1, MPI_INT, prev_rank, 0, MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);
    printf("Process 0 received final value back: %d\n", value);
  } else {
    MPI_Recv(&value, 1, MPI_INT, prev_rank, 0, MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);
    printf("Process %d received value: %d\n", rank, value);

    value += rank;
    printf("Process %d sending value: %d to Process %d\n", rank, value,
           next_rank);
    MPI_Send(&value, 1, MPI_INT, next_rank, 0, MPI_COMM_WORLD);
  }

  // Synchronize before stopping timer
  MPI_Barrier(MPI_COMM_WORLD);
  double end_time = MPI_Wtime();

  if (rank == 0) {
    printf("\nExecution Time with %d process(es): %f seconds\n", size,
           end_time - start_time);
  }

  MPI_Finalize();
  return 0;
}
