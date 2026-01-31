#include <stdio.h>
#include <omp.h>

void simulate_work(int iterations) {
    // This loop makes later iterations take longer than early ones
    volatile int dummy = 0;
    for (long i = 0; i < (long)iterations * 2000000; i++) dummy++;
}

int main() {
    int total_iters = 16;
    omp_set_num_threads(4);

    // 1. STATIC: Predictable, fixed blocks assigned at the start.
    printf("\n--- STATIC SCHEDULING (Chunk 4) ---\n");
    #pragma omp parallel for schedule(static, 4)
    for (int i = 0; i < total_iters; i++) {
        simulate_work(i);
        printf("Thread %d handled iteration %d\n", omp_get_thread_num(), i);
    }

    // 2. DYNAMIC: Threads "grab" new work as soon as they finish a chunk.
    printf("\n--- DYNAMIC SCHEDULING (Chunk 2) ---\n");
    #pragma omp parallel for schedule(dynamic, 2)
    for (int i = 0; i < total_iters; i++) {
        simulate_work(i);
        printf("Thread %d handled iteration %d\n", omp_get_thread_num(), i);
    }

    // 3. GUIDED: Starts with big chunks, shrinks them to handle small leftover tasks.
    printf("\n--- GUIDED SCHEDULING ---\n");
    #pragma omp parallel for schedule(guided)
    for (int i = 0; i < total_iters; i++) {
        simulate_work(i);
        printf("Thread %d handled iteration %d\n", omp_get_thread_num(), i);
    }

    return 0;
}
