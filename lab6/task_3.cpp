#include <iostream>
#include <mpi.h>


int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);
    int rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int msg, flag;
    MPI_Status status;
    MPI_Request request;

    if (rank == 0)
    {
        MPI_Isend(&rank, 1, MPI_INT, (rank + 1) % size, 0, MPI_COMM_WORLD, &request);
        do{ MPI_Test(&request, &flag, &status); } while (!flag);

        MPI_Irecv(&msg, 1, MPI_INT, size - 1, 0, MPI_COMM_WORLD, &request);
        do{ MPI_Test(&request, &flag, &status); } while (!flag);
        std::cout << "0 get message from last process: " << msg << '\n';
    }
    else
    {
        int sender = (rank - 1) % size;
        MPI_Irecv(&msg, 1, MPI_INT, sender, 0, MPI_COMM_WORLD, &request);
        do { MPI_Test(&request, &flag, &status); } while (!flag);
        std::cout << '[' << rank << "]: Receive message: " << msg << '\n';

        MPI_Isend(&rank, 1, MPI_INT, (rank + 1) % size, 0, MPI_COMM_WORLD, &request);
        do{ MPI_Test(&request, &flag, &status); } while (!flag);
    }

    MPI_Finalize();
    return 0;
}