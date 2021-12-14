#include <iostream>
#include "mpi.h"
#include <math.h>

double startTime;
double local_pi, pi;

double f(double x)
{
    return (4 / (1 + (x*x)));
}

int main(int argc, char *argv[])
{
    int size, rank, N;
    MPI_Status stat;
    char buf[100];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
    {
        std::cout << "Enter precise (N): ";
        std::cin >> N;

        startTime = MPI_Wtime();

        for (int i = 1; i < size; i++)
        {
            MPI_Send(&N, 1, MPI_INT, i, i, MPI_COMM_WORLD);
        }
    }
    else
    {
        MPI_Recv(&N, 1, MPI_INT, 0, rank, MPI_COMM_WORLD, &stat);
    }

    if (N > 0)
    {
        double step = 1.0 / N;
        for (double i = rank; i <= N; i += (double)size)
        {
            double fx = f((i + 0.5) * step) * step;
            local_pi += fx;
        }

        if (rank != 0)
        {
            MPI_Send(&local_pi, 1, MPI_DOUBLE, 0, rank, MPI_COMM_WORLD);
        }
        else
        {
            pi = local_pi;
            double bf;
            for (int i = 0; i < size-1; i++)
            {
                MPI_Recv(&bf, 1, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
                 pi += bf;
            }

            std::cout << "Executed PI: " << pi << '\n';
            std::cout << "Difference: " << abs(M_PI - pi) << '\n';
            double endTime = MPI_Wtime();
            double executeTime = endTime-startTime;
            std::cout.precision(5);
            std::cout << executeTime << " sec.\n";
        }
    }
    else
    {
        if (rank == 0)
        {
            std::cout << "N must be more than 0\n";
        }
    }

    MPI_Finalize();

    return EXIT_SUCCESS;
}
