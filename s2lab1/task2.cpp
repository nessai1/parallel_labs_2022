#include <mpi.h>
#include <omp.h>
#include <iostream>
#define THREADS_NUM 5

double el(int i)
{
    double rs = 4.0/(2 * i + 1);
    if (i % 2 == 1)
    {
        rs *= -1;
    }

    return rs;
}

int main(int argc, char** argv)
{
    int prec;
    int size, rank, nthreads;
    MPI_Status status;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    if ( rank == 0 )
    {
        std::cout << "Enter precision: ";
        std::cin >> prec;

        for (int i = 1; i<size; i++ )
        {
            MPI_Send(&prec,1,MPI_INT,i,0,MPI_COMM_WORLD);
        }
    }
    else
    {
        MPI_Recv(&prec,1,MPI_INT,MPI_ANY_SOURCE,0,MPI_COMM_WORLD, &status);
    }

    double bufResult = 0, result = 0;

    /*
     * Особый случай: Количество точечных итераций меньше, чем процессов: разделять вычисления на потоки бесполезно
     */
    if (prec <= size)
    {
        if (rank < prec)
        {
            bufResult = el(rank);
        }
    }
    else
    {
        int procIterations = prec / size;
        int a = procIterations * rank;
        if (rank == size - 1)
        {
            procIterations += prec % size;
        }
        int b = a + procIterations;

        omp_set_num_threads(THREADS_NUM);
        #pragma omp parallel for shared(a,b) reduction(+: bufResult)
        for (int i = a; i < b; i++)
        {
            bufResult += el(i);
        }
    }

    MPI_Reduce(&bufResult, &result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        std::cout << "Result: " << result << '\n';
    }
    MPI_Finalize();
    return 0;
}