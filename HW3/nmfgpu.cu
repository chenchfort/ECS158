#include <iostream>
#include <cuda.h>
#include <math.h>

using namespace std;

#define BLOCKSIZE 16

/*void mat(const float*A , const float* B, float* C, const int N, const int M, const int K) {
      int i,j,l;
          #pragma omp parallel for shared(A,B,C) private(i,j,l)
              for(i=0; i<N; i++) {
                      for(l=0; l<M; l++) {
                                  float a  = A[M*i+l];
                                              for(j=0; j<K; j++) {
                                                              C[K*i + j] += a*B[K*l+j];
                                                                          }
                                                                                  }
                                                                                      }
                                                                                      }*/

__global__ void nmf(float *a, int r, int c, int k, int niters, float *w, float *h)
{
    int row = blockIdx.y*blockDim.y + threadIdx.y;
      int col = blockIdx.x*blockDim.x + threadIdx.x;
        float temp = 0.0;
          float sum = 0.0;
            
            for (int iter = 0; iter < niters; iter++) {
                //compute W
                  if (col < k && row < r) {
                          //ah'
                          sum = 0.0;
                                for (int i = 0; i < c; i++)
                                          sum += a[row*c + i]*h[col*c + i];
                                      temp =  w[row*k+col]*sum;
                                            //whh'
                                            sum = 0.0;
                                                  for (int i = 0; i < c; i++) {
                                                            float sum2 = 0.0;
                                                                    for (int j = 0; j < k; j++) 
                                                                                sum2 += w[row*k + j]*h[j*c + i];
                                                                            sum += sum2*h[col*c+i];
                                                                                  }
                                                        __syncthreads();    
                                                              w[row*k+col] = temp/sum;
                                                                  }
                      __syncthreads();

                          //compute H
                          if (row < k && col < c) {
                                  //w'a
                                  temp = 0.0;
                                        sum = 0.0;
                                              for (int i = 0; i < r; i++)
                                                        sum += w[i*k + row]*a[i*c + col];
                                                    temp = h[row*c + col]*sum;
                                                          //w'wh
                                                          sum = 0;
                                                                for (int i = 0; i < k; i++) {
                                                                          float sum2 = 0.0;
                                                                                  for (int j = 0; j < r; j++) 
                                                                                              sum2 += w[j*k + row]*w[j*k + i];
                                                                                          sum += sum2*h[i*c+col];
                                                                                                }
                                                                      __syncthreads();    
                                                                            h[row*c+col] = temp/sum;
                                                                                }
                              __syncthreads();
                                }
}


void nmfgpu(float *a, int r, int c, int k, int niters, float *w, float *h)
{
    const dim3 block(BLOCKSIZE, BLOCKSIZE);
      const dim3 grid((r + BLOCKSIZE-1) / BLOCKSIZE, (c + BLOCKSIZE-1) / BLOCKSIZE);

        //initialize
        float *dev_w, *dev_h, *dev_a; 
          cudaMalloc((void**)&dev_w, sizeof(float)*r*k);
            cudaMalloc((void**)&dev_h, sizeof(float)*k*c);
              cudaMalloc((void**)&dev_a, sizeof(float)*r*c);
                cudaMemcpy(dev_w, w, sizeof(float)*r*k, cudaMemcpyHostToDevice);
                  cudaMemcpy(dev_h, h, sizeof(float)*k*c, cudaMemcpyHostToDevice);
                    cudaMemcpy(dev_a, a, sizeof(float)*r*c, cudaMemcpyHostToDevice);
                      //
                      //kernel

                      nmf<<<grid, block>>>(dev_a, r, c, k, niters, dev_w, dev_h);
                        cudaThreadSynchronize();
                          //cpy back

                          cudaMemcpy(w, dev_w, sizeof(float)*r*k, cudaMemcpyDeviceToHost);
                            cudaMemcpy(h, dev_h, sizeof(float)*k*c, cudaMemcpyDeviceToHost);

                              //clean up
                              cudaFree(dev_w);
                                cudaFree(dev_h);
                                  cudaFree(dev_a);
}

/*int main()
  {
    srand(1000);
      float *a, *w, *h;
        int r = 3;
          int k = 2;
            int c = 3;
              a = new float[r*c];
                w = new float[r*k];
                  h = new float[k*c];
                    int count = 1;
                      for (int i = 0; i < r*c; i++)
                        {
                            a[i] = count++;
                              }
                                float wh = 0.1;
                                  for (int i = 0; i < r*k; i++)
                                    {
                                        w[i] = wh;
                                            wh+=0.2;
                                              }
                                                wh = 0.1;
                                                  for (int i = 0; i < k*c; i++)
                                                    {
                                                        h[i] = wh;
                                                            wh+=0.2;
                                                              }
                                                                
                                                                nmfgpu(a, r, c, k, 100, w, h);
                                                                  
                                                                  float *res = new float[r*c];
                                                                    for (int i = 0; i<r*c; i++)
                                                                        res[i] = 0;
                                                                          mat(w,h,res,r,k,c);
                                                                            
                                                                            float error = 0;
                                                                              for (int i=0; i<r*c;i++)
                                                                                  error += abs(res[i]-a[i]);
                                                                                    cout << error << endl;
                                                                                      
                                                                                      for (int i=0; i< r; i++) {
                                                                                          for (int j=0; j< c; j++) {
                                                                                                cout << res[i*c+j] << " ";
                                                                                                    }
                                                                                                        cout << endl;
                                                                                                          }
                                                                                                          }*/
