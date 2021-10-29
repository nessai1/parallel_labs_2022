#include <iostream>
#include <mpi.h>


int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);
    int rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int msg, flag;
    MPI_Status status[2];
    MPI_Request request[2];

    for (int i = 0; i < size; i++)
    {
        if (i == rank)
        {
            continue;
        }

        MPI_Isend(&rank, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &request[0]);
        MPI_Irecv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &request[1]);

        MPI_Waitall(2, request, status);
        std::cout << '[' << rank << "]: Receive message: " << msg << '\n';
    }

    MPI_Finalize();
    return 0;
}