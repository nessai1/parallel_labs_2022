#include "mpi.h"
#include <iostream>

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int rank;
    MPI_Status status;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
    {
        char msg[] = "Hello world";
        MPI_Send(msg, 11, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
    }
    else if (rank == 1)
    {
        char msg[11];
        MPI_Recv(msg, 11, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);

        std::cout << "Master set message: " << msg << '\n';
    }

    MPI_Finalize();
    return 0;
}