#include <mpi.h>
#include <stdio.h>
#include <math.h>

int is_prime(int n) {
    if (n < 2) return 0;
    for (int i = 2; i <= sqrt(n); i++) {
        if (n % i == 0) return 0;
    }
    return 1;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int max_val = 10000;
    double start_time = MPI_Wtime();

    if (rank == 0) { // Master
        int current_num = 2;
        int active_slaves = size - 1;
        int msg;
        MPI_Status status;

        while (active_slaves > 0) {
            MPI_Recv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);

            if (msg > 0) {
                // Uncomment to print every prime
                // printf("Prime found: %d\n", msg);
            } // if msg < 0, it's non-prime. if msg == 0, process just started.

            if (current_num <= max_val) {
                MPI_Send(&current_num, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
                current_num++;
            } else {
                int term_signal = -1;
                MPI_Send(&term_signal, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
                active_slaves--;
            }
        }
        printf("Prime search complete. Total time: %f seconds\n", MPI_Wtime() - start_time);
    } else { // Slave
        int num_to_test;
        int response = 0; // 0 indicates just starting

        while (1) {
            MPI_Send(&response, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            MPI_Recv(&num_to_test, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            if (num_to_test == -1) break; // Terminate signal

            if (is_prime(num_to_test)) {
                response = num_to_test;
            } else {
                response = -num_to_test;
            }
        }
    }

    MPI_Finalize();
    return 0;
}
