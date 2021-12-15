//
// Created by daniel on 15.12.2021.
//

#include <iostream>
#include <mpi.h>

int main(int argc, char *argv[])
{
    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank % 2)
    {
        std::cout << "I am " << rank << ":SECOND!\n";
    }
    else
    {
        std::cout << "I am " << rank << ":FIRST!\n";
    }

    if (rank == 0)
    {
        std::cout << size << " processes.\n";
    }

    MPI_Finalize();
    return 0;
}