#include "mpi.h"
#include <iostream>

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int message;
    MPI_Status status;

    if (rank == 0)
    {
        message = rank;
        MPI_Send(&message, 1, MPI_INT, (rank+1) % size, 0, MPI_COMM_WORLD);
    }

    int sender = rank == 0 ? size - 1 : rank-1;
    MPI_Recv(&message, 1, MPI_INT, sender, 0, MPI_COMM_WORLD, &status);

    std::cout << '[' << rank << ']' << ":receive message '" << message << "'\n";

    message++;
    MPI_Send(&message, 1, MPI_INT, (rank+1) % size, 0, MPI_COMM_WORLD);

    MPI_Finalize();
    return 0;
}