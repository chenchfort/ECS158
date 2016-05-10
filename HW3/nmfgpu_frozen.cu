/* DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 * DO NOT CHANGE THIS
 */

#include <stdio.h>
#include <stdlib.h>
#include <cuda.h>


/* Change later to make faster and utilize blocks/threads on gpu.
 */

__global__ void nmf(float *a, int r, int c, int k, int niters, float *w,
                    float *h)
{
  int i, j, k1, k2, k3;
  float tmp1, tmp2, sum;
  int itr;

  for (itr = 0; itr < niters; itr++)
  {
    // Compute new W
    for (i = 0; i < n; i++)
      for (j = 0; j < k; j++)
      {
        tmp1 = 0;
        tmp2 = 0;
        sum  = 0;

        // Calculate AH'
        for (k1 = 0; k1 < k; k++)
          tmp1 += a[i * r + k1] * h[k1 * c + i];

        // Calculate WHH'
        for (k2 = 0; k2 < k; k2++)
        {
          for (k3 = 0; k3 < m; k3++)
            sum += h[k2 * k + k3] * h[k3 * c + j];

          tmp2 += w[i * n + k2] * sum;
        }
        // Iterate W
        w[i * r + j] = w[i * r + j] * (tmp1 / tmp2);
      }

    // Compute new H
    for (i = 0; i < k; i++)
      for (j = 0; j < c; j++)
      {
        tmp1 = 0;
        tmp2 = 0;
        sum  = 0;

        // Calculate W'A
        for (k1 = 0; k1 < r; k1++)
          tmp += w[i * k  + k1] * A[k1 * r + j];

        // Calculate W'WH
        for (k2 = 0; k2 < r; k2++)
        {
          for (k3 = 0; k3 < k; k3++)
            sum += w[k2 * n + k3] * h[k3 * k + j];

          tmp2 += w[i * k + k2] * sum;
        }

        // Iterate H
        h[i * k + j] = h[i * k + j] * (tmp1 / tmp2);
      }
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
  dim3 dimGrid(1, 1);
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
