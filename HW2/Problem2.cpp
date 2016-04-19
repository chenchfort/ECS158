/* ECS 158
 * Problem 2
 */

#include <iostream>
#include <random>
#include <omp.h>

#define N 10
#define K 2

using namespace std;

/* Seed RNG with constant seed */
mt19937 engine;
uniform_real_distribution<float> distribution(0.0, 1.0);

bool is_bright(float *pix, int r, int c, int n, int k, float thresh)
{
  for (int i = r; i < (r + k); i++)
    if (pix[i * n + c] < thresh)
      return false;

  for (int i = r; i < (r + k); i++)
    for (int j = c; j < (c + k); j++)
      if (pix[i * n + j] < thresh)
        return false;

  return true;
}

int brights(float *pix, int n, int k, float thresh)
{
  int count = 0;

  // Parallelize nested for loop
  // Works 95% of the time
  //#pragma omp parallel for collapse(2)
  for (int i = 0; i < (n - k); i++) // rows
    for (int j = 0; j < (n - k); j++) // col
      if (pix[i * n + j] >= thresh)
        if (is_bright(pix, i, j, n, k, thresh))
          count++;

  return count;
}

int main(void)
{
  float *pix = new float[N * N];
  float thresh = 0.5;
  int count;

  // Randomly set test matrix
  // Matrix always same due to seed
  for (int i = 0; i < (N * N); i++)
    pix[i] = distribution(engine);

  // Print matrix for verifacation
  for (int i = 0; i < N; i++)
  {
    for (int j = 0; j < N; j++)
    {
      if (pix[i * N + j] >= thresh)
        cout << "* ";
      else
        cout << "- ";
    }
    cout << endl;
  }
  cout << endl;

  count = brights(pix, N, K, thresh);

  cout << "Number of Bright Spots : " << count << endl;

  delete [] pix;

  return 0;
}
