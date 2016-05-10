#include <stdio.h>
#include <stdlib.h>
#include <cuda.h>


__global__ void nmf(float *a, int r, int c, int k, int niters, float *w,
                    float *h)
{
  int i;
  int r_num = blockIdx.x;
  int c_num = blockIdx.y;

  for (i = 0; i < niters; i++)
  {
  }
}


void nmfgpu(float *a, int r, int c, int k, int niters, float *w, float *h)
{
  int *dev_a, *dev_w, *dev_h;
  int i;
  int a_size = r * c;
  int w_size = k * r;
  int h_size = k * c;

  w = (float*) malloc(w_size * sizeof(float));
  h = (float*) malloc(h_size * sizeof(float));

  // Initial values for w and h
  for (i = 0; i < w_size; i++)
    w[i] = 1;

  for (i = 0; i < h_size; i++)
    h[i] = 1;

  // Allocate memory to GPU
  cudaMalloc((void**) &dev_a, a_size);
  cudaMalloc((void**) &dev_w, w_size);
  cudaMalloc((void**) &dev_h, h_size);

  // Copy a to device
  cudaMemcpy(dev_a, a, a_size, cudaMemcpyHostToDevice);
  cudaMemcpy(dev_w, w, w_size, cudaMemcpyHostToDevice);
  cudaMemcpy(dev_h, h, h_size, cudaMemcpyHostToDevice);

  // Set up threads structure of GPU
  // Play around with this later
  dim3 dimGrid(r, c);
  dim3 dimBlock(1, 1, 1);

  // invoke kernel
  nmf<<<dimGrid, dimBlock>>>(dev_a, r, c, k, niters, dev_w, dev_h);

  // Apply barrier on GPU
  cudaThreadSynchronize();

  // Copy from gpu back to host
  cudaMemcpy(w, dev_w, w_size, cudaMemcpyDeviceToHost);
  cudaMemcpy(h, dev_h, h_size, cudaMemcpyDeviceToHost);

  // Clean up
  cudaFree(dev_a);
  cudaFree(dev_w);
  cudaFree(dev_h);
}
