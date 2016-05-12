#include <iostream>
#include <omp.h>

using namespace std;

int block;

void check_path(int *adjm, int n, int k, int *paths, int *numpaths, int row, int &depth, int *visited)
{
	// Base case
	if (depth == k) {
		#pragma omp critical
		{
			for (int *ptr = paths + (*numpaths) * (k + 1), i = 0; i <= depth; i++, ptr++)
				*ptr = visited[i];

			(*numpaths)++;
		}
		return;
	}

	for (int col = 0; col < n; col++)
	{
		if (adjm[row * n + col] == 1) {
			depth++;
			visited[depth] = row;
			check_path(adjm, n, k, paths, numpaths, col, depth, visited);
			depth--;
      // avoid double count
			if (depth == k - 1)
				break;
		}
	}
}

void findpaths(int *adjm, int n, int k, int *paths, int *numpaths)
{
	*numpaths = 0;
  #pragma omp parallel shared(paths, numpaths)
	{
		int me = omp_get_thread_num();
		int nth = omp_get_num_threads();
    #pragma omp single
    {
      block = n > nth ? (n / nth) : 1;
    }

    #pragma omp paralle for shared(paths, numpaths) collapse(2)
		for (int i = me*block; i < (me + 1)*block && i < n; i++) {
			for (int j = 0; j < n; j++) {
				if (adjm[i * n + j] == 1) {
					int depth = 0;
					int visited[k + 1];
					visited[depth] = i;
					check_path(adjm, n, k, paths, numpaths, j, depth, visited);
				}
			}
		}
	}
}
