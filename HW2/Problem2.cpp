#include <omp.h>

using namespace std;

bool is_brights(float *pix, int i, int j,int n, int k, float thresh)
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

  #pragma omp parallel for reduction(+:count) collapse(2) schedule(guided)
  for (int i = 0; i < n - K + 1; i++)
    for (int j = 0; j < n - k + 1; j++)
      if (pix[i * n + j] > thresh) // Check if pixle is already above thresh
        if (is_brights(pix, i, j, n, k, thresh))
          count++;

  return count;
}
