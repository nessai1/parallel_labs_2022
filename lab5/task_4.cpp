#include "mpi.h"
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <vector>

int main(int argc, char* argv[])
{
    std::srand(std::time(nullptr));
    float b;
    MPI_Init(&argc, &argv);
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Корейский рандом
    float randNumber = static_cast<float>((rank+1) * std::rand()) / static_cast<float>(RAND_MAX);
    std::cout << "Процесс " << rank << " до передачи: " << std::setprecision(2) << randNumber << '\n';

    MPI_Status status;
    if (rank == 0)
    {
        std::vector<float> values(size-1);
        for (int i = 1; i < size; i++)
        {
            MPI_Ssend(&randNumber, 1, MPI_FLOAT, i, 0, MPI_COMM_WORLD);
            float message;
            MPI_Recv(&message, 1, MPI_FLOAT, i, 0, MPI_COMM_WORLD, &status);
            values[i-1] = message;
        }

        std::cout << "Процесс 0: ";
        for (int i = 0; i < size-1; i++)
        {
            std::cout << values[i] << ' ';
        }
        std::cout << '\n';
    }
    else
    {
        float newValue;
        MPI_Recv(&newValue, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Send(&randNumber, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
        std::cout << "Процесс " << rank << ": " << newValue << '\n';
    }
    MPI_Finalize();

    return 0;
}