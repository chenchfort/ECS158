#include <iostream>
#include <omp.h>

using namespace std;

void findpaths_helper(int *adjm, int row, int k, int n, int *paths, int *numpaths, int *depth, const int start);

void findpaths(int *adjm, int n, int k, int *paths, int *numpaths)
{
  int depth;

  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < n; j++)
    {
      if (adjm[i * n + j] == 1)
      {
        // Recurse
        // pass j as row to search
        depth = 1;
        paths[i * n + (paths * numpaths[i] + depth)] = i;
        findpaths_helper(adjm, j, k, n, paths, numpaths, depth, j);
      }
    }
  }
}


void findpaths_helper(int *adjm, int row, int k, int n, int *paths, int *numpaths, int *depth, const int start)
{
  // Base case, k nodes found
  if (depth == k)
    return;

  else
  {
    for (int i = 0; i < n; i++)
    {
      if (adjm[row * n + i] == 1)
      {
        depth++;
        paths[start * n + (paths * numpaths[start] + depth)] = row;
        findpaths_helper(adjm, i, k, paths, numpaths, depth);
        break;
      }
      else
      {
        // reset path stored in numpaths
        for (int i = 0; i < depth; i++)
          paths[start * n + (paths * numpaths[start] + depth)] = 0;
        depth = 0;
        break;
      }
    }
  }
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

  int *numpaths = new int[100];
  int *paths = new int[100];

  findpaths(adjm, n, k, paths, numpaths);

  return 0;
}
