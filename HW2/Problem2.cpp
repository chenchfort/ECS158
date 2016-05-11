#include <omp.h>

using namespace std;

inline bool is_brights(const float *pix, const int i, const int j, const int n,const int k, const float thresh) 
{
	for (int p = i; p < i + k; p++)
		for (int q = j; q < j + k; q++)
			if (pix[p * n + q] < thresh)
				return false;
	return true;
}


int brights(float *pix, int n, int k, float thresh)
{
  int count = 0;
  const int s = n - k + 1;
  #pragma omp parallel for reduction(+:count) collapse(2) schedule(guided)
  for (int i = 0; i < s; i++)
    for (int j = 0; j < s; j++)
      if (pix[i * n + j] >= thresh) // Check if pixle is already above thresh
        if (is_brights(pix, i, j, n, k, thresh))
          count+=1;

  return count;
}

int main(void)
{
  float *pix = new float[N * N];
  float thresh = 0.5;
  int bright_ctr, pix_ctr = 0;
  chrono::time_point<chrono::system_clock> start, end;
  chrono::duration<double> time_elapsed;

  for (int i = 0; i < (N * N); i++)
  {
    pix[i] = distribution(engine);

    if (pix[i] > thresh)
      pix_ctr++;
  }

  start = chrono::system_clock::now();

  bright_ctr = brights(pix, N, K, thresh);

  end = chrono::system_clock::now();

  time_elapsed = end - start;

  cout << "Bright pixles  : " << pix_ctr << endl;
  cout << "From brights() : " << bright_ctr << endl;
  cout << "Time taken     : " << time_elapsed.count() << "s" << endl;
  cout << endl;

  delete [] pix;

  return 0;
}
