#include <stdio.h>
#include <stdlib.h>

// 7. Write the CUDA kernel that computes the sum
__global__ void sumKernel(float *d_array, float *d_result, int N) {
    // Shared memory for block-level reduction
    extern __shared__ float sdata[];

    int tid = threadIdx.x;
    int i = blockIdx.x * blockDim.x + threadIdx.x;

    // Load into shared memory
    sdata[tid] = (i < N) ? d_array[i] : 0.0f;
    __syncthreads();

    // Block-level reduction
    for (int s = blockDim.x / 2; s > 0; s >>= 1) {
        if (tid < s) {
            sdata[tid] += sdata[tid + s];
        }
        __syncthreads();
    }

    // Thread 0 of each block atomically adds to global result
    if (tid == 0) {
        atomicAdd(d_result, sdata[0]);
    }
}

int main() {
    int N = 1000000;
    size_t size = N * sizeof(float);

    float *h_array = (float *)malloc(size);
    float h_result = 0.0f;
    for (int i = 0; i < N; i++) h_array[i] = 1.5f;

    float *d_array, *d_result;

    // 1. Allocate device memory
    cudaMalloc((void **)&d_array, size);
    cudaMalloc((void **)&d_result, sizeof(float));

    // 2. Copy host memory to device
    cudaMemcpy(d_array, h_array, size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_result, &h_result, sizeof(float), cudaMemcpyHostToDevice);

    // 3. Initialize thread block and kernel grid dimensions
    int threadsPerBlock = 256;
    int blocksPerGrid = (N + threadsPerBlock - 1) / threadsPerBlock;
    size_t sharedMemSize = threadsPerBlock * sizeof(float);

    // 4. Invoke CUDA kernel
    sumKernel<<<blocksPerGrid, threadsPerBlock, sharedMemSize>>>(d_array, d_result, N);
    cudaDeviceSynchronize();

    // 5. Copy results from device to host
    cudaMemcpy(&h_result, d_result, sizeof(float), cudaMemcpyDeviceToHost);

    printf("Array Sum: %f\n", h_result);

    // 6. Free device memory
    cudaFree(d_array);
    cudaFree(d_result);
    free(h_array);

    return 0;
}
