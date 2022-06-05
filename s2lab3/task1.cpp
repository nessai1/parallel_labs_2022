#include <mpi.h>
#include <omp.h>
#include <ctime>
#include <iostream>
#include <cmath>
#define MAX_VALUE 100


int main(int argc, char** argv) {
    setlocale(LC_ALL, "ru");
    MPI_Init(&argc, &argv);
    srand(time(nullptr));

    int N, size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);


    double bf;
    double ost = modf(std::sqrt(size), &bf);
    if (ost > 0)
    {
        if (rank == 0)
        {
            std::cout << "[ERROR]: Корень количества процессов не должен иметь дробную часть\n";
        }
        MPI_Finalize();
        exit(MPI_ERR_ARG);
    }

    int s = std::sqrt(size);
    int m;
    if (rank == 0)
    {
        std::cout << "N: ";
        std::cin >> N;
        m = N/s;
    }

    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&s, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (N % s != 0) {
        if (rank == 0)
            std::cout << "N должен быть кратен s (" << s << ")\n";
        MPI_Finalize();
        return MPI_ERR_ARG;
    }

    int *A, *Apart, *V, *C;
    A = new int[N*N];
    Apart = new int[m*m];
    V = new int[N];
    C = new int[N];
    for (int i = 0; i < N; i++) C[i] = 0;

    if (rank == 0)
    {
        std::cout << "Generated matrix\n";
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                A[i*N+j] = rand() % MAX_VALUE;
                std::cout << A[i*N+j] << '\t';
            }
            std::cout << '\n';
        }
        std::cout << "\nGenerated vector\n";
        for (int i = 0; i < N; i++)
        {
            V[i] = rand() % MAX_VALUE;
            std::cout << V[i] << '\n';
        }
    }
    MPI_Bcast(V, N, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Datatype blocktype;
    MPI_Datatype blocktype2;

    MPI_Type_vector(m, m, N, MPI_INT, &blocktype2);

    MPI_Type_create_resized(blocktype2, 0, sizeof(int), &blocktype);
    MPI_Type_commit(&blocktype);
    int *displs, *counts;
    displs = new int[s * s];
    counts = new int[s * s];
    for (int i = 0; i < s; i++) {
        for (int j = 0; j < s; j++) {
            displs[i * s + j] = i * N * m + j * m;
            counts[i * s + j] = 1;
        }
    }

    for (int i = 0; i < m*m; i++)
    {
        Apart[i] = 0;
    }

    MPI_Scatterv(A, counts, displs, blocktype, Apart, m * m, MPI_INT, 0, MPI_COMM_WORLD);

    int block_y = rank / s, block_x = rank % s;

    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < m; j++)
        {
            C[i + block_y * m] += Apart[i * m + j] * V[j + block_x * m];
        }

    }

    if (rank == 0)
        MPI_Reduce(MPI_IN_PLACE, C, N, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    else
        MPI_Reduce(C, C, N, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        std::cout << "\nRESULT VECTOR: \n\n";
        for (int i = 0; i < N; i++)
        {
            std::cout << C[i] << std::endl;
        }
    }

    MPI_Finalize();

    return 0;
}
