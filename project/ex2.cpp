#include <stdio.h>
#include <stdlib.h>
#include <openacc.h>

using namespace std;

void nmfomp(float *a, int r, int c, int k, in niters, float *w, float *h)
{
  for (itr = 0; itr < niters; itr++) {
    #pragma acc kernels 
    for (i = 0; i < n; i++) {
      for (j = 0; j < k; j++) {
        tmp1 = 0;
        tmp2 = 0;
        sum  = 0;
        for (k1 = 0; k1 < k; k++)
          tmp1 += a[i * r + k1] * h[k1 * c + i];
        // Calculate WHH'
        for (k2 = 0; k2 < k; k2++) {
          for (k3 = 0; k3 < m; k3++)
            sum += h[k2 * k + k3] * h[k3 * c + j];
          tmp2 += w[i * n + k2] * sum;
        }
        // Iterate W
        w[i * r + j] = w[i * r + j] * (tmp1 / tmp2);
      }
    }
 
    // Compute new H
    #pragma acc kernels
    for (i = 0; i < k; i++) {
      for (j = 0; j < c; j++) {
        tmp1 = 0;
        tmp2 = 0;
        sum  = 0;
        // Calculate W'A
        for (k1 = 0; k1 < r; k1++)
          tmp += w[i * k  + k1] * A[k1 * r + j];
        // Calculate W'WH
        for (k2 = 0; k2 < r; k2++) {
          for (k3 = 0; k3 < k; k3++)
            sum += w[k2 * n + k3] * h[k3 * k + j];
          tmp2 += w[i * k + k2] * sum;
        }
        // Iterate H
        h[i * k + j] = h[i * k + j] * (tmp1 / tmp2);
      }
    }
  }
}

int main(void)
{
  int n = 10, m = 5;
  return 0;
}
