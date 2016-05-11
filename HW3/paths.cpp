#include <iostream>
#include <vector>

// Detected 4 valid paths
// There should only be 2

using namespace std;

void findpaths_helper(int *adjm, int row, int k, int n, int *paths,
                      int *numpaths, int &depth, bool &found, int *visited);

void findpaths(int *adjm, int n, int k, int *paths, int *numpaths) 
{
  *numpaths = 0;
  for (int row = 0; row < n; row++)
  {
    for (int col = 0; col < n; col++)
    {
      // Link to anther path found
      if (adjm[row * n + col] == 1)
      {
        int depth = 0;
        bool found = false;
        int visited[k + 1];
        visited[0] = row; // Add starting vertex

        findpaths_helper(adjm, col, k, n, paths, numpaths, depth, found,
            visited);
        (*numpaths)++;
      }
    }
  }
  return;
}

void findpaths_helper(int *adjm, int row, int k, int n, int *paths,
                      int *numpaths, int &depth, bool &found, int *visited)
{
  for (int col = 0; col < n; col++)
  {
    if (adjm[row * n + col] == 1)
    {
      depth++;
      visited[depth] = row;
      if (depth - 1 == k)
      {
        for (int *ptr = paths + (*numpaths) * (k + 1), i = 0; 
             i < depth; i++, ptr++)
          *ptr = visited[i];

        found = true;
        return;
      }
      findpaths_helper(adjm, col, k, n, paths, numpaths, depth, found, visited);
      depth--;
    }
  }
  return;
}

int main(void) {
  int n = 4; // Number of vertices
  int k = 2;
  int *adjm = new int[n * n];
  
  for (int i = 0; i < n * n; i++)
    adjm[i] = 0;

  /* C -> D  [ 0 1 1 0 ]
   * ^    ^  [ 0 0 0 1 ]
   * |    |  [ 0 0 0 1 ]
   * A -> B  [ 0 0 0 0 ]
   */
  adjm[1] = 1;
  adjm[2] = 1;
  adjm[7] = 1;
  adjm[11] = 1;

  /* A -> B -> C -| [ 0 1 0 ]
   * ^------------| [ 0 0 1 ]
   *                [ 1 0 0 ]
   * adjm[1] = 1;
   * adjm[5] = 1;
   * adjm[6] = 1;
   */

  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < n; j++)
    {
      cout << adjm[i * n + j] << " ";
    }
    cout << endl;
  }
  cout << endl;

  
  int size = 10;
  int numpaths, paths[size];
  
  for (int i = 0; i < size; i++)
    paths[i] = -1;
  
  findpaths(adjm, n, k, paths, &numpaths);
  
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
