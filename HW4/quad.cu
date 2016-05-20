#include <stdio.h>
#include <stdlib.h>
#include <cuda.h>

#define DEBUG
#define BLOCK 32

__global__ void quad(float *a, int n, float *u, float *v)
{
  int tot_x = gridDim.x * blockDim.x; // Total number of x threads
  int tot_y = gridDim.y * blockDim.y; // Total number of y threads
  int me_x  = blockIdx.x * blockDim.x + threadIdx.x; // x thread number
  int me_y  = blockIdx.y * blockDim.y + threadIdx.y; // y threaqd number

  int i, j;
  float sum = 0;

  // Perform matrix quad v = u'Au
  for (i = me_x; i < n; i += tot_x)
    for (j = me_y; j < n; j += tot_y)
      sum += u[i] * a[i * n + j] * u[j];

  // Add atomically to output
  atomicAdd(v, sum);
}

float gpuquad(float *a, int n, float *u) {
  // Function to perform v = u'Au
    float *da, *du, *dv;
    float v = 0;

    cudaMalloc((void **)&da, n * n * sizeof(float));
    cudaMalloc((void **)&du, n * sizeof(float));
    cudaMalloc((void **)&dv, sizeof(float));

    cudaMemcpy(da, a, n * n * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(du, u, n * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(dv, &v, sizeof(float), cudaMemcpyHostToDevice);

    int size = (n / BLOCK) + 1;

    dim3 dimGrid(size, size);     // Fine tune parameters later
    dim3 dimBlock(BLOCK, BLOCK, 1);

    quad<<<dimGrid, dimBlock>>>(da, n, du, dv);

    cudaThreadSynchronize();

    cudaMemcpy(&v, dv, sizeof(float), cudaMemcpyDeviceToHost);

    cudaFree(da);
    cudaFree(du);
    cudaFree(dv);

    return v;
}

int main(void)
{
  int n = 2;
  float *a = (float*) malloc(n * n * sizeof(float));
  float *u = (float*) malloc(n * sizeof(float));

  a[0] = 1;
  a[1] = 2;
  a[2] = 2;
  a[3] = 4;

  u[0] = 1;
  u[1] = 2;

  #ifdef DEBUG
  // Serial code for testing
  // Possiably true in general
  // Check input with R
  int i, j;
  float sum = 0;
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      sum += u[i] * a[i * n + j] * u[j];
  printf("Solution = %f\n", sum);
  #endif

  float output = gpuquad(a, n, u);

  #ifdef DEBUG
  printf("GPU = %f\n", output);
  #endif

  free(a);
  free(u);

  return 0;
}

