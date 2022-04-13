#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    int size, rank;
    int nthreads, tid;
    MPI_Status status;
    char host[32];
    int i;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    if ( rank == 0 ) {
#pragma omp parallel
        {
            nthreads = omp_get_num_threads();
        }
        for ( i=1; i<size; i++ ) {
            MPI_Send(&nthreads,1,MPI_INT,i,0,MPI_COMM_WORLD);
        }
    } else {
        MPI_Recv(&nthreads,1,MPI_INT,MPI_ANY_SOURCE,0,MPI_COMM_WORLD, &status);
        omp_set_num_threads(nthreads);
    }

    gethostname(host,32);
#pragma omp parallel private(tid,nthreads)
    {
        nthreads = omp_get_num_threads();
        tid = omp_get_thread_num();
        printf("MPI rank %d from %d, host %s. Thread %d from %d.\n",rank,size,host,tid,nthreads);
    }
    MPI_Finalize();
    return 0;
}
