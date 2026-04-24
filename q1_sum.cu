#include <stdio.h>
#include <stdlib.h>

__global__ void multiTaskSum(int *d_out, int n) {
    int tid = threadIdx.x;

    if (tid == 0) {
        // Task A: Iterative Approach
        long long sum = 0;
        for (int i = 1; i <= n; i++) {
            sum += i;
        }
        d_out[0] = sum;
    }
    else if (tid == 1) {
        // Task B: Formula Approach
        d_out[1] = n * (n + 1) / 2;
    }
}

int main() {
    int N = 1024;
    int h_out[2] = {0, 0};
    int *d_out;

    cudaMalloc((void**)&d_out, 2 * sizeof(int));

    // Launch 1 block with 2 threads
    multiTaskSum<<<1, 2>>>(d_out, N);
    cudaDeviceSynchronize();

    cudaMemcpy(h_out, d_out, 2 * sizeof(int), cudaMemcpyDeviceToHost);

    printf("Iterative Sum (Thread 0): %d\n", h_out[0]);
    printf("Formula Sum   (Thread 1): %d\n", h_out[1]);

    cudaFree(d_out);
    return 0;
}
