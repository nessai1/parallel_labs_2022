#include <iostream>
#include "mpi.h"
#include <ctime>
#define ARR_SIZE 5

int main(int argc, char *argv[])
{
    int size, rank, n;
    int A[ARR_SIZE];
    int trash[ARR_SIZE];

    MPI_Status stat;
    MPI_Init(&argc, &argv);

    srand(time(nullptr));

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
    {
        std::cout << "Введите кол-во повторений: ";
        std::cin >> n;
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = 0; i < n; i++)
    {
        if (rank == 0)
        {
            int sender = 1 + (rand() % size);
            for (int j = 1; j < size; j++)
            {
                MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
                if (stat.MPI_SOURCE == sender)
                {
                    MPI_Recv(&A, ARR_SIZE, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
                }
                else
                {
                    MPI_Recv(&trash, ARR_SIZE, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
                }
            }
            std::cout << "Get message from [" << sender << "] \n";
            std::cout << "Message: ";
            for (int j : A)
            {
                std::cout << j << ' ';
            }
            std::cout << '\n';
        }
        else
        {
            for (int & j : A)
            {
                j = (rand() % 1000) * rank / (rank/3+1); // корейский рандом для устранения повторений из-за работы рандомайзера
            }
            MPI_Send(&A, ARR_SIZE, MPI_INT, 0, rank, MPI_COMM_WORLD);
        }

        MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Finalize();

    return EXIT_SUCCESS;
}
