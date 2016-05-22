#include <stdio.h>
#include <time.h>
#include <cstring>
#include <iostream>
#include <mpi.h>
#include <cstdlib>

using namespace std;

#define DATA_MSG 0
#define NEW_MSG 1
#define DATA_MSG2 2
#define NEW_MSG2 3

int nodes, me, chunk, start, end;

int *transgraph(int *adjm, int n, int *nout) //every result data is on node 0
{
    int *outm,
    *num1s,
    *cumul1s;
    
    num1s = (int *)malloc(n*sizeof(int));
    cumul1s = (int *)malloc((n+1)*sizeof(int));
    
    MPI_Comm_size(MPI_COMM_WORLD, &nodes);
    MPI_Comm_rank(MPI_COMM_WORLD, &me);
    
    int myworker = me - 1;
    
    int lenchunk = (n + nodes - 2) / (nodes - 1);
    
    if (me == 0) {
        MPI_Status status;
        for (int i = 1; i < nodes; i++)
            MPI_Recv(num1s + (i-1)*lenchunk, lenchunk, MPI_INT, i, NEW_MSG, MPI_COMM_WORLD, &status);
        
    }
    else {
        
        
        for (int i = myworker*lenchunk; i < (myworker+1)*lenchunk && i < n; i++) {
            int tot1s = 0;
            for (int j = 0; j < n; j++)
                if (adjm[n*i+j] == 1) {
                    adjm[n*i+(tot1s++)] = j;
                }
            num1s[i] = tot1s;
        }  //do actual work
        MPI_Send(num1s + (me-1)*lenchunk, lenchunk, MPI_INT, 0, NEW_MSG, MPI_COMM_WORLD);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    
    int out[2];
    
    if (me == 0) {
        MPI_Status status;
        cumul1s[0] = 0;
        // now calculate where the output of each row in adjm // should start in outm
        for (int m=1;m<=n;m++) {
            cumul1s[m] = cumul1s[m-1] + num1s[m-1];
            
        }
        
        *nout = cumul1s[n];
        for (int i = 1; i < nodes; i++) {
            MPI_Send(num1s, n, MPI_INT, i, DATA_MSG, MPI_COMM_WORLD);
            MPI_Send(cumul1s, n+1, MPI_INT, i, DATA_MSG2, MPI_COMM_WORLD);
        }
        
        outm = (int *)malloc(2*(*nout) * sizeof(int));
        
        for (int i = 0; i < (*nout); i++) {
            MPI_Recv(out, 1, MPI_2INT, MPI_ANY_SOURCE, NEW_MSG, MPI_COMM_WORLD, &status);
            outm[out[0]] = out[1];
            MPI_Recv(out, 1, MPI_2INT, MPI_ANY_SOURCE, NEW_MSG2, MPI_COMM_WORLD, &status);
            outm[out[0]] = out[1];
        }
    }
    else {
        MPI_Status status;
        MPI_Recv(num1s, n, MPI_INT, 0, DATA_MSG, MPI_COMM_WORLD, &status);
        MPI_Recv(cumul1s, n+1, MPI_INT, 0, DATA_MSG2, MPI_COMM_WORLD, &status);
        
        for (int i = myworker*lenchunk; i < (myworker+1)*lenchunk && i < n; i++) {
            int outrow = cumul1s[i];
            int num1si = num1s[i];
            
            for (int j=0; j<num1si; j++){
                out[0] = 2*(outrow+j);
                out[1] = i;
                MPI_Send(out, 1, MPI_2INT, 0, NEW_MSG, MPI_COMM_WORLD);
                out[0] = 2*(outrow+j) + 1;
                out[1] = adjm[n*i+j];
                MPI_Send(out, 1, MPI_2INT, 0, NEW_MSG2, MPI_COMM_WORLD);
            }
        }
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
    
    if (me == 0)
        return outm;
    else
        return NULL;
}

/*
int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    int i,j;
    int *adjm;
    int n = atoi(argv[1]);
    int nout;
    int *outm;
    srand(100);
    adjm = (int *)malloc(n*n*sizeof(int)); for(i=0;i<n;i++)
        for (j = 0; j < n; j++)
            if (i == j)
                adjm[n*i+j] = 0;
            else
                adjm[n*i+j] = rand() % 2;
    struct timespec bgn,nd;
    
    outm = transgraph(adjm, n, &nout);
    
    if (me == 0) {
    printf("number of output rows: %d\n",nout);
    
    if (n<=10)
        for (i = 0; i < nout; i++)
            printf("%d %d\n",outm[2* i ] ,outm[2* i +1]);
    }
    MPI_Finalize();
    return 0;
}*/
