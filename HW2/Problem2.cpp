int brights(float *pix, int n, int k, float thresh)
{
	int count = 0;
	#pragma omp parallel for reduction(+:count)
	for (int i = 0; i < n - k+1; i++)
		for (int j = 0; j < n - k+1; j++) {
			for (int p = i; p < i+k; p++)
				for (int q = j; q < j+k; q++)
					if (pix[p*n + q] < thresh)
						goto fall;
					count+=1;
				fall: ;
		}

	return count;
}
