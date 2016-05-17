#include <iostream>
#include <stdlib.h>
#include <cuda.h>

using namespace std;

#define BLOCKSIZE 32

__global__ void quad(float *a, int n, float *u) {

}

float gpuquad(float *a, int n, float *u) {
    int *da, *du;
    cudaMalloc((void **)&da, n * n);
    cudaMalloc((void **)&du, n);
    cudaMemcpy(da, a, n * n, cudaMemcpyHostToDevice);
    cudaMemcpy(du, u, n, cudaMemcpyHostToDevice);
    dim3 dimGrid(n, 1);
    dim3 dimBlock(1, 1, 1);
    quad<<<dimGrid,dimBlock>>>(da, n, du);
    cudaThreadSynchronize();

    cudaFree(da);
    cudaFree(du);
}

int main ()
{

}

