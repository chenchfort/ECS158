#include <iostream>
#include <omp.h>

using namespace std;

int n_th, chunk;

void check_path(int *adjm, int n, int k, int *paths, int *numpaths, int row, int &depth, int *visited)
{
  // Base case
  if (depth  == k)
  {
    #pragma omp critical
    {
      for (int *ptr = paths + (*numpaths) * (k + 1), i = 0; i <= depth; i++, ptr++)
        *ptr = visited[i];
    }

    (*numpaths)++;
    return;
  }

  for (int col = 0; col < n; col++)
  {
    if (adjm[row * n + col] != 1 && col == (n - 1))
      return;

    if (adjm[row * n + col] == 1)
    {
      depth++;
      visited[depth] = row;

      check_path(adjm, n, k, paths, numpaths, col, depth, visited);
      depth--;

      // Avoid duplicates
      if (depth == k - 1)
        break;
    }
  }
}

void findpaths(int *adjm, int n, int k, int *paths, int *numpaths)
{
  // Iterate to check through all the rows and col

  #pragma omp parallel
  {
    int n_paths = 0;
    int t_id = omp_get_thread_num();

    #pragma omp single
    {
      // Partition chunk
      n_th = omp_get_num_threads();
      chunk = (n * n) / n_th;
    }

    int start = t_id * chunk;
    int end   = start + chunk - 1;

    for (int i = start; i < end; i++)
    {
      if ( adjm[i] == 1)
      {
        int col = i % n;
        int depth = 0;
        int visited[k + 1];
        visited[depth] = col;
        check_path(adjm, n, k, paths, &n_paths, col, depth, visited);
      }
    }

    #pragma omp critical
    {
      // Accumate the number of paths
      (*numpaths) += n_paths;
    }
  }
}

int main(void)
{
  int n = 4;
  int k = 2;

  int *adjm = new int[n * n];

  for (int i = 0; i < n * n; i++)
    adjm[i] = 0;

  adjm[1]  = 1;
  adjm[2]  = 1;
  adjm[6]  = 1;
  adjm[7]  = 1;
  adjm[11] = 1;
  adjm[12] = 1;

  int size = 300;
  int numpaths, paths[size];

  for (int i = 0; i < size; i++)
    paths[i] = -1;

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      cout << adjm[i * n + j] << " ";
    }
    cout << endl;
  }

  findpaths(adjm, n, k, paths, &numpaths);
  cout << numpaths << endl;

  for (int i = 0; i < numpaths; i++)
  {
      for (int j = 0; j < k + 1; j++)
      {
        cout << paths[i * (k + 1) + j] << " ";
      }
      cout << endl;
  }

  return 0;
}
