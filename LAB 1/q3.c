#include <stdio.h>
#include <omp.h>

int main() {
    long num_steps = 100000000;
    double step = 1.0 / (double)num_steps;
    double pi, sum = 0.0;
    double start = omp_get_wtime();

    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < num_steps; i++) {
        double x = (i + 0.5) * step; 
        sum = sum + 4.0 / (1.0 + x * x);
    }

    pi = step * sum;
    double end = omp_get_wtime();

    printf("Pi: %f, Time: %f\n", pi, end - start);

    return 0;
}
