#include <mpi.h>
#include <omp.h>
#include <unistd.h>
#include <iostream>

int main(int argc, char** argv)
{
    int size, rank, nthreads;
    MPI_Status status;
    int i;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    if ( rank == 0 )
    {
        std::cin >> nthreads;
        for ( i=1; i<size; i++ )
        {
            MPI_Send(&nthreads,1,MPI_INT,i,0,MPI_COMM_WORLD);
        }
    }
    else
    {
        MPI_Recv(&nthreads,1,MPI_INT,MPI_ANY_SOURCE,0,MPI_COMM_WORLD, &status);
        omp_set_num_threads(nthreads);
    }

    #pragma omp parallel num_threads(nthreads)
    {
        int numberOfThreads = omp_get_num_threads();
        int threadNumber = omp_get_thread_num();
        #pragma omp critical
        std::cout << "I am " << threadNumber << " thread from " << rank << " process. Number Of Hybrid Threads = " <<  size * numberOfThreads << '\n';
    }
    MPI_Finalize();
    return 0;
}