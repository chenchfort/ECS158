#include <iostream>
#include <cuda.h>
#include <math.h>
#include <fstream>
using namespace std;

#define BLOCKSIZE 32

//test code
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

__global__ void nmfw(float *a, int r, int c, int k, float *w, float *h, float *wcp)//must be block synchronized!!!
{
	int row = blockIdx.y*blockDim.y + threadIdx.y;
	int col = blockIdx.x*blockDim.x + threadIdx.x;
	
	//compute W
	if (col < k && row < r) {
		//ah'
		float sum = 0.0;
		float temp = 0.0;
		for (int i = 0; i < c; i++)
			sum += a[row*c + i]*h[col*c + i];
		temp =  w[row*k+col]*sum;
		//whh'
		sum = 0.0;
		for (int i = 0; i < c; i++) {
			for (int j = 0; j < k; j++) {
				sum += w[row*k + j]*h[j*c + i]*h[col*c+i];
			}
		}
		__syncthreads();
		wcp[row*k+col] = temp/sum;
	}
}

__global__ void nmfh(float *a, int r, int c, int k, float *w, float *h, float *hcp)//must be block synchronized!!!
{
	int row = blockIdx.y*blockDim.y + threadIdx.y;
	int col = blockIdx.x*blockDim.x + threadIdx.x;
	
	//compute H
	if (row < k && col < c) {
		//w'a
		float temp = 0.0;
		float sum;
		sum = 0.0;
		for (int i = 0; i < r; i++)
			sum += w[i*k + row]*a[i*c+col];

		temp = h[row*c+col]*sum;
		//w'wh
		sum = 0.0;
		for (int i = 0; i < k; i++)
			for (int j = 0; j < r; j++) 
				sum += w[j*k + row]*w[j*k + i]*h[i*c+col];

		__syncthreads();		
		hcp[row*c+col] = temp/sum;
	}
}

__global__ void nmfcpy(float *mat, float *matcp, int m, int n) //kernel copy must be block synchronized!!!
{
	int row = blockIdx.y*blockDim.y + threadIdx.y;
	int col = blockIdx.x*blockDim.x + threadIdx.x;
	
	if (row < m && col < n)
		mat[row*n+col] = matcp[row*n+col];
}

void nmfgpu(float *a, int r, int c, int k, int niters, float *w, float *h)
{
	const dim3 block(BLOCKSIZE, BLOCKSIZE);
	const dim3 grid((c + BLOCKSIZE - 1)/ BLOCKSIZE,(r + BLOCKSIZE - 1)/ BLOCKSIZE);
	//initialize
	float *dev_w, *dev_h, *dev_a, *dev_wcp, *dev_hcp; 
	cudaMalloc((void**)&dev_w, sizeof(float)*r*k);
	cudaMalloc((void**)&dev_h, sizeof(float)*k*c);
	cudaMalloc((void**)&dev_wcp, sizeof(float)*r*k);
	cudaMalloc((void**)&dev_hcp, sizeof(float)*k*c);
	cudaMalloc((void**)&dev_a, sizeof(float)*r*c);
	cudaMemcpy(dev_w, w, sizeof(float)*r*k, cudaMemcpyHostToDevice);
	cudaMemcpy(dev_h, h, sizeof(float)*k*c, cudaMemcpyHostToDevice);
	cudaMemcpy(dev_a, a, sizeof(float)*r*c, cudaMemcpyHostToDevice);
	//
	//kernel
	for (int i=0; i<niters; i++) { //slow way
		nmfw<<<grid, block>>>(dev_a, r, c, k, dev_w, dev_h, dev_wcp);
		cudaThreadSynchronize();
		nmfcpy<<<grid, block>>>(dev_w, dev_wcp, r, k);
		cudaThreadSynchronize();
		nmfh<<<grid, block>>>(dev_a, r, c, k, dev_w, dev_h, dev_hcp);
		cudaThreadSynchronize();
		nmfcpy<<<grid, block>>>(dev_h, dev_hcp, k, c);
		cudaThreadSynchronize();
	}
	
	//cpy back
	cudaMemcpy(w, dev_w, sizeof(float)*r*k, cudaMemcpyDeviceToHost);
	cudaMemcpy(h, dev_h, sizeof(float)*k*c, cudaMemcpyDeviceToHost);

	//clean up
	cudaFree(dev_w);
	cudaFree(dev_h);
	cudaFree(dev_a);
}

//test code, u can test it if u want
/*int main()
{
	srand(1000);
	float *w, *h;
	const int r = 194;
	int k = 50;

	const int c = 259;

	w = new float[r*k];
	h = new float[k*c];

	float a[r*c];
	ifstream file("af.txt");
	for (int i = 0; i < 194 * 259; i++)
		file >> a[i];
	
	for (int i = 0; i < r*k; i++)
	{
		w[i] = (float)rand()/RAND_MAX;
	}
	for (int i = 0; i < k*c; i++)
	{
		h[i] = (float)rand()/RAND_MAX;
	}

	nmfgpu(a, r, c, k, 100, w, h);
	
	float *res = new float[r*c];

	for (int i = 0; i<r*c; i++)
		res[i] = 0;

	mat(w,h,res,r,k,c);
	
	ofstream output("result.txt");

	for (int i=0; i < r; i++) {
		for (int j=0; j <c; j++)
			output << res[i*c+j] << " ";
		output << "\n";
	}
	
	
}*/
