#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
s
#define MSG 0

// Declare global variables for MPI
int nodes, me, chunk, start, end;

void findmyrange(int n, int nth, int me, int *myrange)
{
  int chunksize = n/nth;
  myrange[0] = me *chunksize;
  if (me < nth - 1)
    myrange[1] = (me+1) * chunksize - 1;
  else
    myrange[1] = n - 1;
}

void init()
{
  MPI_Comm_size(MPI_COMM_WORLD, &nodes);
  MPI_Comm_rank(MPI_COMM_WORLD, &me);
  chunk = nv/nnodes;
  start = me * chunk;
  end = start + chunk-1;
  //malloc
}

int *transgraph(int *adjm, int n, int *nout)
{
  
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
