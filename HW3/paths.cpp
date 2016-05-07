#include <iostream>
#include <omp.h>

using namespace std;


void findpaths(int *adjm, int n, int k, int *paths, int *numpaths)
{
  for (int i = 0; i < n; i++)
  { // Scan through rows
    for (int j = 0; j < n; j++)
    { // Scan through col
      if (adjm[i * n + j] == 1)
    }
  }
    

  return;
}






int main(void)
{
  // Test matrix, 1 -> 2 -> 3.
  // k = 2
  // result should be 3.

  int n = 3; // Number of vertices
  int k = 2;
  int *adjm = new int[n * n];

  for (int i = 0; i < n * n; i++)
    adjm[i] = 0;

  adjm[1] = 1;
  adjm[5] = 1;
  adjm[6] = 1;

  findpaths(adjm, n, k, paths, numpaths);

  return 0;
}

