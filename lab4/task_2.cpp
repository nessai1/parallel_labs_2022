#include <iostream>
#include <mpi.h>

int main(int argc, char *argv[])
{
    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    std::cout << "I am " << rank << " process from " << size << " processes!!\n";

    MPI_Finalize();
    return 0;
}