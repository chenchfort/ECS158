#include <stdio.h>
#include <stdlib.h>
#include <string.h>

float quad(float *a, int n, float *u)
{
	float res = 0;
	int i, n2 = n*n;
	#pragma acc loop reduction(+:res)
	for (i = 0; i < n2; i++) {
		int row = i / n;
		int col = i % n;
		if (row < n && col < n && col >= row) {
			float sum = u[col] * a[row*n + col] * u[row];
			if (col == row)
				res += sum;
			else
				res += 2 * sum;
		}
	}

	return res;
}

int main(void) {

	int n = 2;
	float *a = (float*)malloc(n * n * sizeof(float));
	float *u = (float*)malloc(n * sizeof(float));

	a[0] = 1;
	a[1] = 2;
	a[2] = 2;
	a[3] = 4;

	u[0] = 1;
	u[1] = 2;

	int i, j;
	float sum = 0;
	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			sum += u[i] * a[i * n + j] * u[j];
	printf("Solution = %f\n", sum);

	float output = quad(a, n, u);
	printf("GPU = %f\n", output);
}
