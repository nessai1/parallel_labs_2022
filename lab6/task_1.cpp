#include <mpi.h>
#include <iostream>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, flag;
    MPI_Status status;
    MPI_Request request;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        char msg[] = "Hello world";
        MPI_Isend(msg, 11, MPI_CHAR, 1, 0, MPI_COMM_WORLD, &request);

        do
        {
            MPI_Test(&request, &flag, &status);
        } while (!flag);
        std::cout << "[0]: Message send\n";
    }
    else if (rank == 1) {
        char msg[11];
        MPI_Irecv(msg, 11, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &request);
        do
        {
            MPI_Test(&request, &flag, &status);
        } while (!flag);
        std::cout << "[1]: Message receive from 0 process: '" << msg << "'\n";
    }

    MPI_Finalize();
    return 0;
}