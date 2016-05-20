#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// Declare global variables for MPI
int n_nodes, my_rank, chunk, start, end;

int *transgraph(int *adjm, int n, int *nout)
{
  // Set nout to 0 incase caller does not set it
  (*nout) = 0;

  int *out_matrix, *num_1s, *cumul_1s;
  int myrows[2];
  int tot_1s, out_row, num_1s_i;

  num_1s = (int*) malloc(n * sizeof(int));
  cumul_1s = (int*) malloc((n + 1) * sizeof(int));

  // Scatter work across nodes
  for (i = 0; i < n; i++)
  {
    for (j = 0; j < n; j++)
    {
      find_my_min();
      MPI_Reduce();
      MPI_Bcast();
    }
  }

  // Gather work
  MPI_Gather();

  return out_matrix;
}

int main(int argc, char **argv)
{
  // Set up adjancy matrix
  int nout  = 0;
  int nv    = 10;
  int *adjm = (int*) malloc(n * n * sizeof(int));

  for (int i = 0; i < nv; i++)
    for (int j = 0; j < nv; j++)
      if (i == j) adjm[i * n + j] = 0;
      else        adjm[i * n + j] = rand() % 2;

  // Initalize MPI
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &n_nodes);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  // Partition out the work in chunks
  chunk = nv / n_nodes;
  start = me * chunk;
  end   = start + chunk - 1;

  int *out_matrix = transgraph(adjm, nv, nout);

  // Finalize
  MPI_Finalize();

  return 0;
}
