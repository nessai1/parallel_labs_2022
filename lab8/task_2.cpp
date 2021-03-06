#include <iostream>
#include "mpi.h"

int A[200], B[200], Cbuf[200], Crecv[200], C[200], Arecv[200];
double startTime;

int main(int argc, char *argv[])
{
    int size, rank, n;
    MPI_Status stat;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
    {
        std::cout << "Enter size of square matrix (n): ";
        std::cin >> n;
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    int rest = n % size;
    int Asize = (n <= size) ? size : ((n/size + (rest == 0 ? 0 : 1)) * size);
    int step = Asize / size;

    if (rank == 0)
    {
        std::cout << "Enter matrix A:\n";
        if (Asize == size || rest == 0)
        {
            for (int i = 0; i < n; i++)
            {
                for (int j = 0; j < n; j++)
                {
                    std::cin >> A[i*n + j];
                }
            }
        }
        else
        {

            for (int i = 0; i < Asize; i++)
            {
                if ((i / step) >= rest && i % step == (step-1) && size != 1)
                {
                    continue;
                }

                for (int j = 0; j < n; j++)
                {
                    std::cin >> A[i*n + j];
                }
            }
        }

        std::cout << "Enter matrix B:\n";
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                std::cin >> B[i*n + j];
            }
        }
        startTime = MPI_Wtime();
    }

    MPI_Bcast(&B, n*n, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank == 0)
    {
        for (int i = 1; i < size; i++)
        {
            MPI_Send(A+(i*step*n), step*n, MPI_INT, i, i, MPI_COMM_WORLD);
        }
        for (int i = 0; i < step*n; i++)
        {
            Arecv[i] = A[i];
        }
    }
    else
    {
        MPI_Recv(&Arecv, step*n, MPI_INT, 0, rank, MPI_COMM_WORLD, &stat);
    }

    for (int i = 0; i < step; i++)
    {
        for (int j = 0; j < n; j++)
        {
            int el = 0;
            for (int q = 0; q < n; q++)
            {
                el += Arecv[(i*n)+q]*B[j+(q*n)];
            }
            Cbuf[i*n + j] = el;
        }
    }

    if (rank == 0)
    {
        for (int i = 0; i < step*n; i++)
        {
            C[i] = Cbuf[i];
        }
        for (int i = 1; i < size; i++)
        {
            MPI_Recv(&Crecv, step*n, MPI_INT, i, i, MPI_COMM_WORLD, &stat);
            for (int j = 0; j < step*n; j++)
            {
                C[i*step*n+j] = Crecv[j];
            }
        }
    }
    else
    {
        MPI_Send(&Cbuf, step*n, MPI_INT, 0, rank, MPI_COMM_WORLD);
    }

    if (rank == 0)
    {
        std::cout << "----- RESULT -----\n";
        for (int i = 0; i < Asize; i++)
        {
            if ((i / step) >= rest && i % step == (step-1) && size != 1 && rest != 0)
            {
                continue;
            }

            for (int j = 0; j < n; j++)
            {
                std::cout << C[i*n + j] << ' ';
            }
            std::cout << '\n';
        }
        std::cout << '\n';

        double endTime = MPI_Wtime();
        double executeTime = endTime-startTime;
        std::cout.precision(12);
        std::cout << executeTime << " sec.\n";
    }
    MPI_Finalize();

    return EXIT_SUCCESS;
}