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
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&buf, n, MPI_CHAR, 0, MPI_COMM_WORLD);
    for (int i = rank; i < n; i += size)
    {
        symbols[getPos(buf[i])]++;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Reduce(&symbols, &rs, 26, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        for (int i = 0; i < 26; i++)
        {
            if (rs[i] > 0)
            {
                std::cout << getSymbol(i) << '=' << rs[i] << '\n';
            }
        }
        double endTime = MPI_Wtime();
        double executeTime = endTime-startTime;
        std::cout.precision(5);
        std::cout << executeTime << " sec.\n";
    }
    MPI_Finalize();

    return EXIT_SUCCESS;
}
