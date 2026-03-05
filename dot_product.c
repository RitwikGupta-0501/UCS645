#include <mpi.h>
#include <stdio.h>

#define VECTOR_SIZE 8

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int chunk_size = VECTOR_SIZE / size;

  int A[VECTOR_SIZE] = {1, 2, 3, 4, 5, 6, 7, 8};
  int B[VECTOR_SIZE] = {8, 7, 6, 5, 4, 3, 2, 1};

  int local_A[chunk_size];
  int local_B[chunk_size];

  if (rank == 0) {
    printf("Vector A: ");
    for (int i = 0; i < VECTOR_SIZE; i++)
      printf("%d ", A[i]);
    printf("\nVector B: ");
    for (int i = 0; i < VECTOR_SIZE; i++)
      printf("%d ", B[i]);
    printf("\n\n");
  }

  // Distribute both vectors to all processes
  MPI_Scatter(A, chunk_size, MPI_INT, local_A, chunk_size, MPI_INT, 0,
              MPI_COMM_WORLD);
  MPI_Scatter(B, chunk_size, MPI_INT, local_B, chunk_size, MPI_INT, 0,
              MPI_COMM_WORLD);

  // Each process computes its partial dot product
  int local_dot = 0;
  for (int i = 0; i < chunk_size; i++) {
    local_dot += local_A[i] * local_B[i];
  }
  printf("Process %d: partial dot product = %d\n", rank, local_dot);

  // Reduce all partial dot products to get final result
  int global_dot = 0;
  MPI_Reduce(&local_dot, &global_dot, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    printf("\nDot Product : %d\n", global_dot);
    printf("Expected    : 120\n");
  }

  MPI_Finalize();
  return 0;
}
