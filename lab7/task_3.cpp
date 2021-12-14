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
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (N > 0)
    {
        double step = 1.0 / N;
        for (double i = rank; i <= N; i += (double)size)
        {
            double fx = f((i + 0.5) * step) * step;
            local_pi += fx;
        }

        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Reduce(&local_pi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

        if (rank == 0)
        {
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
