#include <omp.h>
#include <mpi.h>
#include <iostream>
#define N 1000
#define NUM_DIMS 1

double* T(const double *A) {
    auto *A_T = new double[N*N];
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            A_T[i * N + j] = A[j * N + i];
        }
    }
    return A_T;
}

int main(int argc, char** argv) {
    setlocale(LC_ALL, "ru");
    MPI_Init(&argc, &argv);

    int p, rank, dims[NUM_DIMS], reorder = 0, periods[NUM_DIMS], source, dest;
    double t1,t2;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm comm_cart;
    MPI_Status st;
    if (N % p != 0) {
        if (rank == 0) {
            std::cout << "ERROR: Incorrect size\n";
        }
        MPI_Finalize();
        exit(MPI_ERR_ARG);
    }
    int k = N / p;

    double *A = (double*) malloc(N*N * sizeof(double));
    double *B = (double*) malloc(N*N * sizeof(double));
    double *C = (double*) malloc(N*N * sizeof(double));
    double *Ccalc = (double*) malloc(N*N * sizeof(double));
    double *B_T = new double[k * N];
    if (rank == 0)
    {

        FILE *fpA, *fpB, *fpC;
        fpA = fopen("A.dat", "rb");
        fpB = fopen("B.dat", "rb");
        fpC = fopen("C.dat", "rb");

        fread(A, sizeof(double), N*N, fpA);
        fread(B, sizeof(double), N*N, fpB);
        fread(C, sizeof(double), N*N, fpC);

        fclose(fpA);
        fclose(fpB);
        fclose(fpC);

        B_T = T(B);
        t1 = MPI_Wtime();
    }

    auto *Apart = new double[N*k];
    auto *Bpart = new double[N*k];
    auto *Cpart = new double[N*k];
    MPI_Scatter(A, N*k, MPI_DOUBLE, Apart, N*k, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(B_T, N*k, MPI_DOUBLE, Bpart, N*k, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    /* Обнуляем массив dims и заполняем засеяв periods для топологии "кольцо" */
    for(int i= 0; i < NUM_DIMS; i++) {
        dims[i] = 0; periods[i] = 1;
    }
    /* Заполняем массив dims, где указываются размеры (одномерной) решетки */
    MPI_Dims_create(p, NUM_DIMS, dims);
/* Создаем топологию "кольцо" с communicator(om) comm_cart */
    MPI_Cart_create(MPI_COMM_WORLD, NUM_DIMS, dims, periods, reorder,&comm_cart);
/* Каждая ветвь находит своих соседей вдоль кольца, в направлении
* больших значений рангов */
    MPI_Cart_shift(comm_cart, 0, 1, &source, &dest);

    for (int i = 0; i < N * k; i++) Cpart[i] = 0;
    for (int i = 0; i < k; i++)
    {
        for (int j = 0; j < k; j++)
        {
            for (int q = 0; q < N; q++)
            {
                Cpart[(i * N) + (k * rank) + j] += Apart[i*N + q] * Bpart[j*N + q];
            }
        }
    }

    auto *Bbuf = new double[N*k];
    for (int q = rank+1; q < rank+p; q++)
    {
        MPI_Sendrecv(Bpart, N*k, MPI_DOUBLE, source, 0, Bbuf, N * k, MPI_DOUBLE, dest, 0, comm_cart, &st);
        for (int i = 0; i < N * k; i++)
        {
            Bpart[i] = Bbuf[i];
        }
        for (int i = 0; i < k; i++)
        {
            for (int j = 0; j < k; j++)
            {
                for (int t = 0; t < N; t++)
                {
                    Cpart[i*N + k * (q % p) + j] += Apart[i * N + t] * Bpart[j*N + t];
                }
            }
        }
    }
    MPI_Gather(Cpart, N*k, MPI_DOUBLE, Ccalc, N*k, MPI_DOUBLE, 0, comm_cart);

    if (rank == 0)
    {
        std::cout << "N = " << N << '\n';
        t2 = MPI_Wtime();
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                if (Ccalc[i*N + j] != C[i*N + j])
                {
                    std::cout << "Result is not correct: C["  << i << "][" << j << "] = " << C[i*N+j] << " != " << Ccalc[i*N+j] << '\n';
                    exit(MPI_ERR_OTHER);
                }
            }
        }

        std::cout << "RESULT IS CORRECT: " << p << " process execute in " << t2-t1 << '\n';
    }

    MPI_Finalize();
    return 0;
}
