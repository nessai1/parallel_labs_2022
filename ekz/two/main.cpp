#include <iostream>
#include <mpi.h>
#include <ctime>
#define ARR_SIZE 10



using namespace std;




int main(int argc, char* argv[]) {
    float A[ARR_SIZE];
    float result[ARR_SIZE];

    srand(time(0));
    MPI_Init(&argc, &argv);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    for (int i = 0; i < ARR_SIZE; i++)
    {
        A[i] = ((float)i + static_cast<float>(rand())/ (static_cast<float>(RAND_MAX/(world_rank+1+ARR_SIZE-i))))*(world_rank+rand()%ARR_SIZE);
    }

    for (int i = 0; i < world_size; i++)
    {
        if (i == world_rank)
        {
            std::cout << "RANK " << i << " ELEMENTS:\n";
            for (int j = 0; j < ARR_SIZE; j++)
            {
                std::cout << A[j] << ' ';
            }
            std::cout << '\n';
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Reduce(&A, &result, ARR_SIZE, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (world_rank == 0)
    {
        std::cout << "SUM RESULT:\n";
        for (int i = 0; i < ARR_SIZE; i++)
        {
            std::cout << result[i] << ' ';
        }
        std::cout << '\n';
    }


    MPI_Finalize();
}