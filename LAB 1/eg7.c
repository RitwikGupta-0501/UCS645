#include <stdio.h>
#include <omp.h>

int main() {
    int shared_counter = 0;

    #pragma omp parallel
    {
        // Code outside the critical section runs in parallel
        
        #pragma omp critical
        {
            // Code inside here is executed by only one thread at a time
            shared_counter++;
            printf("Thread %d entered critical section. Counter: %d\n", 
                    omp_get_thread_num(), shared_counter);
        }
    }

    printf("Final Counter Value: %d\n", shared_counter);
    return 0;
}
