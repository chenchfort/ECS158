#include <stdio.h>
#include <stdlib.h>
#include <cuda.h>

#define BLOCK 16

__global__ void quad(float *a, int n, float *u, float *v)
{
  int col  = blockIdx.x * blockDim.x + threadIdx.x;
  int row  = blockIdx.y * blockDim.y + threadIdx.y;

  if (row < n && col < n && col >= row) {
	float sum = u[col]*a[row*n+col]*u[row];
	if (col == row)
		atomicAdd(v, sum);
	else
		atomicAdd(v, 2*sum);
  }
}

float gpuquad(float *a, int n, float *u) {
    float *da, *du, *dv;
    float v = 0;

    cudaMalloc((void **)&da, n * n * sizeof(float));
    cudaMalloc((void **)&du, n * sizeof(float));
    cudaMalloc((void **)&dv, sizeof(float));

    cudaMemcpy(da, a, n * n * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(du, u, n * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(dv, &v, sizeof(float), cudaMemcpyHostToDevice);

    int size = (n+BLOCK-1) / BLOCK;

    dim3 dimGrid(size, size);
    dim3 dimBlock(BLOCK, BLOCK);

    quad<<<dimGrid, dimBlock>>>(da, n, du, dv);
    cudaMemcpy(&v, dv, sizeof(float), cudaMemcpyDeviceToHost);

    cudaFree(da);
    cudaFree(du);
    cudaFree(dv);

    return v;
}
