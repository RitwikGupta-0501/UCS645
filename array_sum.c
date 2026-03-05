#include <mpi.h>
#include <stdio.h>

#define ARRAY_SIZE 100

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int chunk_size = ARRAY_SIZE / size;
  int array[ARRAY_SIZE];
  int local_array[chunk_size];

  if (rank == 0) {
    // Initialize array with values 1 to 100
    for (int i = 0; i < ARRAY_SIZE; i++) {
      array[i] = i + 1;
    }
    printf("Array initialized with values 1 to %d\n\n", ARRAY_SIZE);
  }

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

  if (rank == 0) {
    printf("\nGlobal Sum  : %d\n", global_sum);
    printf("Expected    : 5050\n");
    // Bonus: compute average
    double average = (double)global_sum / ARRAY_SIZE;
    printf("Average     : %.2f\n", average);
  }

  MPI_Finalize();
  return 0;
}
