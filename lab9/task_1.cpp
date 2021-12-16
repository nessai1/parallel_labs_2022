#include <iostream>
#include <mpi.h>
#include <algorithm>

int main(int argc, char *argv[])
{
    int size, rank;
    MPI_Comm newcomm;
    MPI_Group oldgroup, group;
    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_group(MPI_COMM_WORLD, &oldgroup);

    int count = (size/2) + (size%2);
    int* evenThreads = new int[count];
    for (int i = 0; i < size; i += 2)
        evenThreads[i/2] = i;

    MPI_Comm_group(MPI_COMM_WORLD, &oldgroup);
    MPI_Group_incl(oldgroup, count, evenThreads, &group);
    MPI_Comm_create(MPI_COMM_WORLD, group, &newcomm);

    if (newcomm != MPI_COMM_NULL)
    {
        char msg[10];
        std::fill(msg, msg + 10, 0);

        int N;
        int newrank, newsize;

        MPI_Comm_size(newcomm, &newsize);
        MPI_Comm_rank(newcomm, &newrank);

        if (newrank == 0)
        {
            std::cout << "Enter N: ";
            std::cin >> N;
            std::cout << "Enter message: ";
            std::cin >> msg;
        }

        MPI_Bcast(&N, 1, MPI_INT, 0, newcomm);
        MPI_Bcast(&msg, N, MPI_CHAR, 0, newcomm);
        std::cout << "MPI_COMM_WORLD: " << rank << " from " << size << ". Newcomm: " << newrank << " from " << newsize << ". Message = " << msg << '\n';
    }

    MPI_Finalize();
    return 0;
}