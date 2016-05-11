#include <iostream>
#include <omp.h>

#define VERBOSE

using namespace std;

void check_path(int *adjm, int n, int k, int *paths, int *numpaths, int row, int &depth, int *visited)
{
  // Base case
  if (depth  == k)
  {
    #ifdef VERBOSE
    cout << "    Writing to paths : ";
    #endif

    for (int *ptr = paths + (*numpaths) * (k + 1), i = 0; i <= depth; i++, ptr++)
    {
      *ptr = visited[i];
      #ifdef VERBOSE
      cout << visited[i] << " ";
      #endif
    }
    #ifdef VERBOSE
    cout << endl;
    #endif
    (*numpaths)++;
    return;
  }

  for (int col = 0; col < n; col++)
  {
    if (adjm[row * n + col] != 1 && col == (n - 1))
      return;

    if (adjm[row * n + col] == 1)
    {
      #ifdef VERBOSE
      cout << "  Connection found in vertex " << row << " to vertex " << col << endl;
      #endif
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
  *numpaths = 0;
  // Iterate to check through all the rows and col
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      if (adjm[i * n + j] == 1) // Jump to row j
      {
        int depth = 0;
        int visited[k + 1];
        visited[depth] = i;
        #ifdef VERBOSE
        cout << "Searching vertex " << i << " connected to vertex " << j << endl;
        #endif
        check_path(adjm, n, k, paths, numpaths, j, depth, visited);
      }
    }
  }
}

int main(void)
{
  int n = 3;
  int k = 2;

  int *adjm = new int[n * n];

  for (int i = 0; i < n * n; i++)
    adjm[i] = 0;

  adjm[1] = 1;
  adjm[3] = 1;
  adjm[5] = 1;
  adjm[6] = 1;

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
