#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// CUDA Kernel for merging sub-arrays
__global__ void gpu_merge(int *arr, int *temp, int n, int width) {
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    int left = tid * 2 * width;

    if (left < n) {
        int mid = min(left + width, n);
        int right = min(left + 2 * width, n);

        int i = left, j = mid, k = left;

        while (i < mid && j < right) {
            if (arr[i] <= arr[j]) temp[k++] = arr[i++];
            else temp[k++] = arr[j++];
        }
        while (i < mid) temp[k++] = arr[i++];
        while (j < right) temp[k++] = arr[j++];

        for (int idx = left; idx < right; idx++) {
            arr[idx] = temp[idx];
        }
    }
}

// CPU Merge Sort (Baseline)
void cpu_merge(int *arr, int l, int m, int r) {
    int n1 = m - l + 1, n2 = r - m;
    int *L = (int*)malloc(n1 * sizeof(int));
    int *R = (int*)malloc(n2 * sizeof(int));

    for (int i = 0; i < n1; i++) L[i] = arr[l + i];
    for (int j = 0; j < n2; j++) R[j] = arr[m + 1 + j];

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];

    free(L); free(R);
}

void cpu_merge_sort(int *arr, int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        cpu_merge_sort(arr, l, m);
        cpu_merge_sort(arr, m + 1, r);
        cpu_merge(arr, l, m, r);
    }
}

int main() {
    int n = 1000;
    size_t size = n * sizeof(int);

    int *h_arr_cpu = (int*)malloc(size);
    int *h_arr_gpu = (int*)malloc(size);

    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        h_arr_cpu[i] = h_arr_gpu[i] = rand() % 10000;
    }

    // CPU Timed Execution
    clock_t start_cpu = clock();
    cpu_merge_sort(h_arr_cpu, 0, n - 1);
    clock_t end_cpu = clock();
    double time_cpu = ((double)(end_cpu - start_cpu)) / CLOCKS_PER_SEC;

    // GPU Timed Execution
    int *d_arr, *d_temp;
    cudaMalloc(&d_arr, size);
    cudaMalloc(&d_temp, size);
    cudaMemcpy(d_arr, h_arr_gpu, size, cudaMemcpyHostToDevice);

    cudaEvent_t start_gpu, stop_gpu;
    cudaEventCreate(&start_gpu); cudaEventCreate(&stop_gpu);

    cudaEventRecord(start_gpu);
    int threadsPerBlock = 256;
    for (int width = 1; width < n; width *= 2) {
        int blocks = (n + (threadsPerBlock * 2 * width) - 1) / (threadsPerBlock * 2 * width);
        if (blocks == 0) blocks = 1;
        gpu_merge<<<blocks, threadsPerBlock>>>(d_arr, d_temp, n, width);
        cudaDeviceSynchronize();
    }
    cudaEventRecord(stop_gpu);
    cudaEventSynchronize(stop_gpu);

    float time_gpu = 0;
    cudaEventElapsedTime(&time_gpu, start_gpu, stop_gpu);
    cudaMemcpy(h_arr_gpu, d_arr, size, cudaMemcpyDeviceToHost);

    printf("CPU Merge Sort Time: %f seconds\n", time_cpu);
    printf("GPU Merge Sort Time: %f seconds\n", time_gpu / 1000.0f);

    cudaFree(d_arr); cudaFree(d_temp);
    free(h_arr_cpu); free(h_arr_gpu);
    return 0;
}
