
#include <sys/time.h>
#include <inttypes.h>
#include <cstring>
#include <omp.h>
#include <iostream>
#include <time.h>
#include <stdlib.h>


using namespace std;

#define m(i,j,n) i*n + j


void matMul(float *a, float *b, float *c, int m, int n, int p);

void mWiki(const float*A , const float* B, float* C, const int N, const int M, const int K) {

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
}
void transpose(float *a, int m, int n)
{
	float *at = new float[m*n];
	int i, j;
	
	#pragma omp parallel for shared(a,at) private(i,j)	
	for (j=0; j<n; j++) //col
		for (i=0; i<m; i++) //row
			at[j*m+i] = a[i*n+j];
			
	memcpy(a, at, sizeof(float)*m*n);
	delete[] at;
}

inline void matVec(const float* a, float* cVec, const float* bVec, int m, int n){
    int i, j;
    float val[8], b;
    int temp2;
    
    for(i=0; i<m/8;i++) {
		memset(val, 0.0, sizeof(float)*8);
        
        temp2 = i*8;
        for(j=0;j<n;j++) {
        	b = bVec[j];
        	int temp = temp2*n + j;
            val[0] += a[temp]*b;
            val[1] += a[temp+n]*b;
            val[2] += a[temp+2*n]*b;
            val[3] += a[temp+3*n]*b;
            val[4] += a[temp+4*n]*b;
            val[5] += a[temp+5*n]*b;
            val[6] += a[temp+6*n]*b;
            val[7] += a[temp+7*n]*b;
        }
        
        cVec[temp2] += val[0];
        cVec[temp2+1] += val[1];
        cVec[temp2+2] += val[2];
        cVec[temp2+3] += val[3];
        cVec[temp2+4] += val[4];
        cVec[temp2+5] += val[5];
        cVec[temp2+6] += val[6];
        cVec[temp2+7] += val[7];        
    }
    temp2 = i*8;
    for(i=temp2; i<m; i++) //continue the work
        for(j=0; j<n; j++)
            cVec[i] += a[i*n+j]*bVec[j];
}

void nmfomp(float *a, int r, int c, int k, int niters, float *w, float *h)
{
	int i,j;
	const int size = r*k > k*c ? r*k: k*c;
	float *up = new float[size];
	float *down = new float[size];
	float *temp = new float[k*k];	
	float *wt = new float[r*k];
	
	for (int iter=0; iter < niters; iter++) {
	
		//intialize
		memset(up, 0.0, sizeof(float)*size);
		memset(down, 0.0, sizeof(float)*size);
		memset(temp, 0.0, sizeof(float)*k*k);
		
		////////////////////////w
		#pragma omp parallel for shared(h,up,a)
		for (i=0; i<r; i++) //a*b, b is supposed to be transposed
			matVec(h, up+i*k, a+i*c, k, c);
	
		#pragma omp parallel for shared(h,temp)
		for (i=0; i<k; i++)
			matVec(h, temp+i*k, h+i*c, k, c); 
	
		matMul(w, temp, down, r, k, k);
		
		#pragma omp parallel for shared(w,up,down)
		for (i=0; i<r*k; i++) //update w
			w[i] = w[i]*up[i]/down[i];
		
		//intialize
		memset(up, 0.0, sizeof(float)*size);
		memset(down, 0.0, sizeof(float)*size);
		memset(temp, 0.0, sizeof(float)*k*k);
		memcpy(wt, w, sizeof(float)*r*k);	
		transpose(wt, r, k);
								
		////////////////////////h
		matMul(wt, a, up, k, r, c);
		matMul(wt, w, temp, k, r, k);
		matMul(temp, h, down, k, k, c);
		
		#pragma omp parallel for shared(h,up,down)
		for (i=0; i<k*c; i++) //update h
			h[i] = h[i]*up[i]/down[i];	
		/////////////////////////
	
	}
	delete[] up;
	delete[] down;
	delete[] temp;
	delete[] wt;
}

int main()
{
	float *a, *w, *h;
 	int r = 500;
 	int k = 100;
 	
 	int c = 500;
 	
	a = new float[r*c];
	w = new float[r*k];
	h = new float[k*c];
	int count=1;
	for (int i=0; i<r*c; i++)
	{
   	 	a[i] = count++;
	}
	for (int i=0; i<r*k; i++)
	{
		w[i] = 1;
	}
	
	for (int i=0; i<k*c; i++)
	{
		h[i] = 1;
	}
	
 	double wtime = omp_get_wtime();
 
	
	nmfomp(a,r,c,k,10,w,h);

		
	wtime = omp_get_wtime() - wtime;
	
	cout << wtime << endl;

	delete[] a;
	delete[] w;
	delete[] h;
}

void matMul(float *a, float *b, float *c, int m, int n, int p)
{
	int i,j,k;
	float *bt = new float[n*p]; //bt transpose
	
	#pragma omp parallel for shared(b,bt) private(i,j)	
	for (j=0; j<p; j++) //col
		for (i=0; i<n; i++) //row
			bt[j*n+i] = b[i*p+j];
	
	#pragma omp parallel for shared(a,b,c)
	for (i=0; i<m; i++)
		matVec(bt, c+i*p, a+i*n, p, n); //p*n matrix bt
	
	delete[] bt;
}


