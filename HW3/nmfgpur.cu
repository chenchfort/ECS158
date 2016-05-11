#include <cuda.h>
#include <stdlib.h>
#include <math.h>
#include <Rinternals.h>
#include <iostream>

using namespace std;
// treat it as C code
extern "C" {
    SEXP gpu(SEXP ra, SEXP rk);
}

void mat(const double*A , const double* B, double* C, const int N, const int M, const int K) {
    int i,j,l;

    for(i=0; i<N; i++) {
        for(l=0; l<M; l++) {
            double a = A[M*i+l];
            for(j=0; j<K; j++) {
                C[K*i + j] += a*B[K*l+j];
            }
        }
    }
}

#define BLOCKSIZE 32

void nmfInit(double *a, int r, int c, int k, double *res);

SEXP gpu(SEXP ra, SEXP rk) {
	
    int k = INTEGER(rk)[0]; //k
    SEXP adim = 
       getAttrib(ra, R_DimSymbol);
    int m = INTEGER(adim)[0]; //how many rows
	int n = INTEGER(adim)[1];
	
    double *a = REAL(ra);
	SEXP rres = PROTECT(allocMatrix(REALSXP, m, n));
	double *res = REAL(rres);
	
	for (int i=0; i< m*n; i++)
		res[i] = 0.0;

	double *tmp = new double[m*n];
	double *tmp2 = new double[m*n];

	for (int i=0; i<m*n; i++)
		tmp2[i]= 0;
	
	for (int i=0; i<m; i++)
		for (int j=0; j<n; j++)
			tmp[i*n+j] = a[j*m+i];
	
	nmfInit(tmp, m, n, k, tmp2);

	for (int i=0; i<m; i++)
		for (int j=0; j<n; j++)
			res[j*m+i] = tmp2[i*n+j];
		
    return rres;
}

__global__ void nmfw(double *a, int r, int c, int k, double *w, double *h, double *wcp)//must be block synchronized!!!
{
	int row = blockIdx.y*blockDim.y + threadIdx.y;
	int col = blockIdx.x*blockDim.x + threadIdx.x;
	
	//compute W
	if (col < k && row < r) {
		//ah'
		double sum = 0.0;
		double temp = 0.0;
		for (int i = 0; i < c; i++)
			sum += a[row*c + i]*h[col*c + i];
		temp = w[row*k+col]*sum;
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

__global__ void nmfh(double *a, int r, int c, int k, double *w, double *h, double *hcp)//must be block synchronized!!!
{
	int row = blockIdx.y*blockDim.y + threadIdx.y;
	int col = blockIdx.x*blockDim.x + threadIdx.x;
	
	//compute H
	if (row < k && col < c) {
		//w'a
		double temp = 0.0;
		double sum;
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

__global__ void nmfcpy(double *mat, double *matcp, int m, int n) //kernel copy must be block synchronized!!!
{
	int row = blockIdx.y*blockDim.y + threadIdx.y;
	int col = blockIdx.x*blockDim.x + threadIdx.x;
	
	if (row < m && col < n)
		mat[row*n+col] = matcp[row*n+col];
}

void nmfInit(double *a, int r, int c, int k, double *tmp)
{
	const dim3 block(BLOCKSIZE, BLOCKSIZE);
	const dim3 grid((c + BLOCKSIZE - 1)/ BLOCKSIZE,(r + BLOCKSIZE - 1)/ BLOCKSIZE);
	const int niters = 100;
	srand(time(0));

	//initialize
	double *w = new double[r*k];
	double *h = new double[k*c];
	for (int i = 0; i < r*k; i++)
		w[i] = (double)rand()/RAND_MAX;
	for (int i = 0; i < k*c; i++)
		h[i] = (double)rand()/RAND_MAX;
	//initialize
	double *dev_w, *dev_h, *dev_a, *dev_wcp, *dev_hcp;

	cudaMalloc((void**)&dev_w, sizeof(double)*r*k);
	cudaMalloc((void**)&dev_h, sizeof(double)*k*c);
	cudaMalloc((void**)&dev_wcp, sizeof(double)*r*k);
	cudaMalloc((void**)&dev_hcp, sizeof(double)*k*c);
	cudaMalloc((void**)&dev_a, sizeof(double)*r*c);
	cudaMemcpy(dev_w, w, sizeof(double)*r*k, cudaMemcpyHostToDevice);
	cudaMemcpy(dev_wcp, w, sizeof(double)*r*k, cudaMemcpyHostToDevice);
	cudaMemcpy(dev_h, h, sizeof(double)*k*c, cudaMemcpyHostToDevice);
	cudaMemcpy(dev_hcp, h, sizeof(double)*k*c, cudaMemcpyHostToDevice);
	cudaMemcpy(dev_a, a, sizeof(double)*r*c, cudaMemcpyHostToDevice);
	//
	//kernel
	for (int i=0; i<niters; i++) { //slow way
		nmfw<<<grid, block>>>(dev_a, r, c, k, dev_w, dev_h, dev_wcp);
		nmfcpy<<<grid, block>>>(dev_w, dev_wcp, r, k);
		nmfh<<<grid, block>>>(dev_a, r, c, k, dev_w, dev_h, dev_hcp);
		nmfcpy<<<grid, block>>>(dev_h, dev_hcp, k, c);
	}
	
	//cpy back
	cudaMemcpy(w, dev_w, sizeof(double)*r*k, cudaMemcpyDeviceToHost);
	cudaMemcpy(h, dev_h, sizeof(double)*k*c, cudaMemcpyDeviceToHost);

	//clean up
	cudaFree(dev_w);
	cudaFree(dev_h);
	cudaFree(dev_a);

	mat(w, h, tmp, r, k, c);
}


