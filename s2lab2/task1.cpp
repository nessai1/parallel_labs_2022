#include <iostream>
#include <ctime>
#include <omp.h>
#include <mpi.h>

#define MAX_VALUE 100

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    srand(time(nullptr));

    int n, size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        do {
            std::cout << "Enter N: ";
            std::cin >> n;
            if (n < size || n % size != 0) {
                std::cout << "N >= p-size and N mod p-size = 0\n";
            }
        } while (n < size || n % size != 0);

    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    int *A, *V, *Arecv;
    A = new int[n*n];
    Arecv = new int[n*n];
    V = new int[n];

    if (rank == 0)
    {
        for (int i = 0; i < n*n; i++)
        {
            A[i] = rand() % MAX_VALUE;
        }
        for (int i = 0; i < n; i++)
        {
            V[i] = rand() % MAX_VALUE;
        }
        std::cout << "\n\n\tGenerated matrix\n";
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                std::cout << A[j * n + i] << ' ';
            }
            std::cout << '\n';
        }

        std::cout << "\n\n\tGenerated Vector\n";
        for (int i = 0; i < n; i++)
        {
            std::cout << V[i] << '\n';
        }
        std::cout << "--------------------------\n";
    }

    MPI_Bcast(V, n, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(A, n*(n/size), MPI_INT, Arecv, n*(n/size), MPI_INT, 0, MPI_COMM_WORLD);

    int *Vcalc, *Vrs;
    Vcalc = new int[n];
    Vrs = new int[n];

    for (int i = 0; i < n; i++)
    {
        Vcalc[i] = 0;
    }

#pragma omp for
    for (int i = 0; i < (n/size); i++)
    {
        for (int j = 0; j < n; j++)
        {
            Vcalc[j] += V[i+((n/size)*rank)] * Arecv[j + (n*i)];
        }
    }
    MPI_Reduce(Vcalc, Vrs, n, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    if (rank == 0)
    {
        std::cout << "\nResult vector:\n";
        for (int i = 0; i < n; i++)
        {
            std::cout << Vrs[i] << '\n';
        }
    }
    MPI_Finalize();
    return 0;
}
