#include <stdio.h>

int main() {
    int deviceCount;
    cudaGetDeviceCount(&deviceCount);

    if (deviceCount == 0) {
        printf("No CUDA-capable devices found.\n");
        return 1;
    }

    for (int dev = 0; dev < deviceCount; ++dev) {
        cudaDeviceProp deviceProp;
        cudaGetDeviceProperties(&deviceProp, dev);

        printf("\nDevice %d: \"%s\"\n", dev, deviceProp.name);
        printf("  Compute capability:                            %d.%d\n", deviceProp.major, deviceProp.minor);
        printf("  Total amount of global memory:                 %.2f GB\n", (float)deviceProp.totalGlobalMem / (1024*1024*1024));
        printf("  Maximum block dimensions:                      (%d, %d, %d)\n", deviceProp.maxThreadsDim[0], deviceProp.maxThreadsDim[1], deviceProp.maxThreadsDim[2]);
        printf("  Maximum grid dimensions:                       (%d, %d, %d)\n", deviceProp.maxGridSize[0], deviceProp.maxGridSize[1], deviceProp.maxGridSize[2]);
        printf("  Maximum threads per block:                     %d\n", deviceProp.maxThreadsPerBlock);
        printf("  Total amount of constant memory:               %lu KB\n", deviceProp.totalConstMem / 1024);
        printf("  Total amount of shared memory per block:       %lu KB\n", deviceProp.sharedMemPerBlock / 1024);
        printf("  Warp size:                                     %d\n", deviceProp.warpSize);
    }
    return 0;
}
