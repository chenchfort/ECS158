#include <stdio.h>
#include <stdlib.h>
#include <cuda.h>

#define BLOCKSIZE 32

__global__ void quad(float *a, int n, float *u, float *out)
{
  int tot_th = gridDim.x * blockDim.x; //Total number of threads
  int t_id   = blockIdx.x * blockDim.x + threadIdx.x; //Thread number
  int i, j;
  float sum = 0;

  #ifdef DEBUG
  printf("Thread : %d out of %d\n", t_id, tot_th);
  #endif

  // Perform matrix quad
  for (i = t_id; i < n; i += tot_th)
    for (j = 0; j < n; j++)
      sum += u[i] * a[i * n + j] * u[j];

  atomicAdd(out, sum);
}

float gpuquad(float *a, int n, float *u) {
    float *da, *du, *dout;
    float hout = 0;

    cudaMalloc((void **)&da, n * n * sizeof(float));
    cudaMalloc((void **)&du, n * sizeof(float));
    cudaMalloc((void **)&dout, sizeof(float));

    cudaMemcpy(da, a, n * n * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(du, u, n * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(dout, &hout, sizeof(float), cudaMemcpyHostToDevice);

    dim3 dimGrid(1, 1);
    dim3 dimBlock(n, 1, 1);

    quad<<<dimGrid, dimBlock>>>(da, n, du, dout);

    cudaThreadSynchronize();

    cudaMemcpy(&hout, dout, sizeof(float), cudaMemcpyDeviceToHost);

    cudaFree(da);
    cudaFree(du);

    return hout;
}

int main(void)
{
  int n = 2;
  int i, j;
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

