#include <stdio.h>
#include <stdlib.h>

#define N 1000000

// 1.1 Statically defined global variables
__device__ float d_A[N];
__device__ float d_B[N];
__device__ float d_C[N];

__global__ void vectorAddStatic() {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < N) {
        d_C[i] = d_A[i] + d_B[i];
    }
}

int main() {
    size_t size = N * sizeof(float);
    float *h_A = (float*)malloc(size);
    float *h_B = (float*)malloc(size);
    float *h_C = (float*)malloc(size);

    for (int i = 0; i < N; i++) {
        h_A[i] = 1.0f;
        h_B[i] = 2.0f;
    }

    // Copy to static device memory using symbols
    cudaMemcpyToSymbol(d_A, h_A, size);
    cudaMemcpyToSymbol(d_B, h_B, size);

    // 1.2 Record timing data
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    int threadsPerBlock = 256;
    int blocksPerGrid = (N + threadsPerBlock - 1) / threadsPerBlock;

    cudaEventRecord(start);
    vectorAddStatic<<<blocksPerGrid, threadsPerBlock>>>();
    cudaEventRecord(stop);

    cudaEventSynchronize(stop);
    float milliseconds = 0;
    cudaEventElapsedTime(&milliseconds, start, stop);
    float seconds = milliseconds / 1000.0f;

    printf("Kernel Execution Time: %f ms\n", milliseconds);

    cudaMemcpyFromSymbol(h_C, d_C, size);

    // 1.3 Theoretical Bandwidth
    cudaDeviceProp prop;
    cudaGetDeviceProperties(&prop, 0); // Assuming Device 0

    // memoryClockRate is in kHz, multiply by 1000 for Hz.
    // memoryBusWidth is in bits. Double pumped (x2).
    // Formula: (ClockRate * 1000 * BusWidth * 2) / (8 bits/byte) / 10^9 to get GB/s
    double memClockHz = prop.memoryClockRate * 1000.0;
    double theoreticalBW = (memClockHz * prop.memoryBusWidth * 2.0) / (8.0 * 1e9);

    printf("Theoretical Bandwidth: %f GB/s\n", theoreticalBW);

    // 1.4 Measured Bandwidth
    // Reads: Array A and Array B (2 * N * sizeof(float))
    // Writes: Array C (N * sizeof(float))
    double RBytes = 2.0 * N * sizeof(float);
    double WBytes = 1.0 * N * sizeof(float);
    double measuredBW = ((RBytes + WBytes) / seconds) / 1e9; // Convert to GB/s

    printf("Measured Bandwidth: %f GB/s\n", measuredBW);

    free(h_A); free(h_B); free(h_C);
    return 0;
}
