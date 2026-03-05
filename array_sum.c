#include <mpi.h>
#include <stdio.h>

#define ARRAY_SIZE                                                             \
  104 // Padded to nearest multiple of 8 (works for 1,2,4,8 processes)
#define ACTUAL_SIZE 100 // Actual data size (1 to 100)

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int chunk_size = ARRAY_SIZE / size;
  int array[ARRAY_SIZE];
  int local_array[chunk_size];

  if (rank == 0) {
    // Fill first 100 elements with 1-100, pad rest with 0
    for (int i = 0; i < ACTUAL_SIZE; i++) {
      array[i] = i + 1;
    }
    for (int i = ACTUAL_SIZE; i < ARRAY_SIZE; i++) {
      array[i] = 0; // padding
    }
    printf("Array initialized with values 1 to %d (padded to %d for even "
           "distribution)\n\n",
           ACTUAL_SIZE, ARRAY_SIZE);
  }

  // Synchronize all processes before timing
  MPI_Barrier(MPI_COMM_WORLD);
  double start_time = MPI_Wtime();

  // Distribute array portions to all processes
  MPI_Scatter(array, chunk_size, MPI_INT, local_array, chunk_size, MPI_INT, 0,
              MPI_COMM_WORLD);

  // Each process computes its local sum
  int local_sum = 0;
  for (int i = 0; i < chunk_size; i++) {
    local_sum += local_array[i];
  }
  printf("Process %d: local sum = %d\n", rank, local_sum);

  // Reduce all local sums to get global sum
  int global_sum = 0;
  MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  // Synchronize before stopping timer
  MPI_Barrier(MPI_COMM_WORLD);
  double end_time = MPI_Wtime();

  if (rank == 0) {
    printf("\nGlobal Sum  : %d\n", global_sum);
    printf("Expected    : 5050\n");
    double average = (double)global_sum / ACTUAL_SIZE;
    printf("Average     : %.2f\n", average);
    printf("\nExecution Time with %d process(es): %f seconds\n", size,
           end_time - start_time);
  }

  MPI_Finalize();
  return 0;
}
