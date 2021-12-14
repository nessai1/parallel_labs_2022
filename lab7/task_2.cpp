#include <iostream>
#include "mpi.h"

int symbols[26];
int rs[26];
double startTime;

int getPos(int asciiPos)
{
    int a = 'a';
    return asciiPos - a;
}

char getSymbol(int alphabetPos)
{
    char a = 'a';
    return (char)(a+alphabetPos);
}

int main(int argc, char *argv[])
{
    int size, rank, n;
    MPI_Status stat;
    char buf[100];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
    {
        std::cout << "Enter count of line (n): ";
        std::cin >> n;
        std::cout << "Enter line: ";
        std::cin >> buf;

        startTime = MPI_Wtime();

        for (int i = 1; i < size; i++)
        {
            MPI_Send(&n, 1, MPI_INT, i, i, MPI_COMM_WORLD);
            MPI_Send(&buf, n, MPI_CHAR, i, i, MPI_COMM_WORLD);
        }
    }
    else
    {
        MPI_Recv(&n, 1, MPI_INT, 0, rank, MPI_COMM_WORLD, &stat);
        MPI_Recv(&buf, n, MPI_CHAR, 0, rank, MPI_COMM_WORLD, &stat);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    for (int i = rank; i < n; i += size)
    {
        symbols[getPos(buf[i])]++;
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0)
    {
        for (int i = 0; i < 26; i++)
        {
            for (int j = 1; j < size; j++)
            {
                int symbol;
                MPI_Recv(&symbol, 1, MPI_INT, j, j, MPI_COMM_WORLD, &stat);
                symbols[i] += symbol;
            }
        }
        for (int i = 0; i < 26; i++)
        {
            if (symbols[i] > 0)
            {
                std::cout << getSymbol(i) << '=' << symbols[i] << '\n';
            }
        }

        double endTime = MPI_Wtime();
        double executeTime = endTime-startTime;
        std::cout.precision(5);
        std::cout << executeTime << " sec.\n";
    }
    else
    {
        for (int i = 0; i < 26; i++)
        {
            MPI_Send(&symbols[i], 1, MPI_INT, 0, rank, MPI_COMM_WORLD);
        }
    }
    MPI_Finalize();

    return EXIT_SUCCESS;
}
