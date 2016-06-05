#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define vl 1024

void nmfw(float *a, int r, int c, int k, float *w, float *h, float *wcp, int index)//must be block synchronized!!!
{
	int row = index / k;
	int col = index % k;
	int i, j;

	if (col < k && row < r) {
		//ah'
		float sum = 0.0;
		float temp = 0.0;
		for (i = 0; i < c; i++)
			sum += a[row*c + i] * h[col*c + i];
		temp = w[row*k + col] * sum;
		//whh'
		sum = 0.0;
		for (i = 0; i < c; i++) {
			for (j = 0; j < k; j++) {
				sum += w[row*k + j] * h[j*c + i] * h[col*c + i];
			}
		}

		wcp[row*k + col] = temp / sum;
	}
}

void nmfh(float *a, int r, int c, int k, float *w, float *h, float *hcp, int index)
{
	int row = index / c;
	int col = index % c;
	int i, j;
	//compute H
	if (row < k && col < c) {
		//w'a
		float temp = 0.0;
		float sum;
		sum = 0.0;
		for (int i = 0; i < r; i++)
			sum += w[i*k + row] * a[i*c + col];

		temp = h[row*c + col] * sum;
		//w'wh
		sum = 0.0;
		for (i = 0; i < k; i++)
			for (j = 0; j < r; j++)
				sum += w[j*k + row] * w[j*k + i] * h[i*c + col];

		hcp[row*c + col] = temp / sum;
	}
}

void nmfcpy(float *mat, float *matcp, int m, int n, int i) //kernel copy must be block synchronized!!!
{
	int row = i / n;
	int col = i % n;

	if (row < m && col < n)
		mat[row*n + col] = matcp[row*n + col];
}

void nmfacc(float*a, int r, int c, int k, int niters, float *w, float *h)
{
	int i, j, iter;
	int dim = r*c;
	float *wcp = (float *)malloc(sizeof(float)*r*k);
	float *hcp = (float *)malloc(sizeof(float)*k*c);
	
#pragma acc data copy(w[0:r*k], h[0:k*c]) copyin(a[0:r*c], wcp[0:r*k], hcp[0:k*c])
	for (iter = 0; iter < niters; iter++) 
	{
#pragma acc loop 
		for (i = 0; i < dim; i++)
			nmfw(a, r, c, k, w, h, wcp, i);
#pragma acc loop 
		for (i = 0; i < dim; i++)
			nmfcpy(w, wcp, r, k, i);
#pragma acc loop 
		for (i = 0; i < dim; i++)
			nmfh(a, r, c, k, w, h, hcp, i);
#pragma acc loop 
		for (i = 0; i < dim; i++)
			nmfcpy(h, hcp, k, c, i);
	}
}



int main()
{

	int i;
	float *w, *h;
	const int r = 3;
	int k = 1;

	const int c = 3;

	w = (float *)malloc(sizeof(float)*r*k);
	h = (float *)malloc(sizeof(float)*k*c);
	
	float a[9];
	
	for (i = 0; i < 9; i++)
		a[i] = i + 1;
	
	for (i = 0; i < r*k; i++)
	{
		w[i] = (float)rand() / RAND_MAX;
	}
	for (int i = 0; i < k*c; i++)
	{
		h[i] = (float)rand() / RAND_MAX;
	}

	nmfacc(a, r, c, k, 1, w, h);
	
	printf("%f\n", w[0]);
}
