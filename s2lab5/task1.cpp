#include <omp.h>
#include <mpi.h>
#include <iostream>
#include <math.h>

#define N 500

int main (int argc, char** argv)
{
    setlocale(LC_ALL, "ru");
    MPI_Init(&argc, &argv);

    int size, rank;
    double t1,t2;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Comm grid_comm;
    MPI_Status st1, st2;
    int q = std::sqrt(size);

    if (((double)std::sqrt(size) - q) != 0 || N % size != 0) {
        if (rank == 0) {
            std::cout << "ERROR: Incorrect size\n";
        }
        MPI_Finalize();
        exit(MPI_ERR_ARG);
    }
    int k = N/q;

    int source, dest, reorder = 1;
    int dimensions[2];
    int wrap_around[2];

    dimensions[0] = dimensions[1] = q;
    wrap_around[0] = wrap_around[1] = 1;

    MPI_Cart_create(MPI_COMM_WORLD, 2, dimensions, wrap_around, reorder, &grid_comm);
    int coordinates[2];

    int my_grid_rank;
    //возвращаем ранг процесса в новом коммуникаторе
    MPI_Comm_rank(grid_comm, &my_grid_rank);
    //Чтобы процесс мог определить свои координаты, он должен вызвать функцию MPI_Cart_coords()
    MPI_Cart_coords(grid_comm, my_grid_rank, 2,    coordinates);
    //определяет соседей для процесса, в котором вызывается функция
    MPI_Cart_shift(grid_comm, 0, -1, &source, &dest);

    double *A = (double*) malloc(N*N * sizeof(double));
    double *B = (double*) malloc(N*N * sizeof(double));
    double *C = (double*) malloc(N*N * sizeof(double));
    double *Ccalc = (double*) malloc(N*N * sizeof(double));
    if (my_grid_rank == 0)
    {

        FILE *fpA, *fpB, *fpC;
        fpA = fopen("A_500.dat", "rb");
        fpB = fopen("B_500.dat", "rb");
        fpC = fopen("C_500.dat", "rb");

        fread(A, sizeof(double), N*N, fpA);
        fread(B, sizeof(double), N*N, fpB);
        fread(C, sizeof(double), N*N, fpC);

        fclose(fpA);
        fclose(fpB);
        fclose(fpC);
        t1 = MPI_Wtime();
    }

    MPI_Datatype blocktype;
    MPI_Datatype blocktype2;

    MPI_Type_vector(k, k, N, MPI_DOUBLE, &blocktype2);
    MPI_Type_create_resized(blocktype2, 0, sizeof(double), &blocktype);
    MPI_Type_commit(&blocktype);

    double *Apart = new double[k*k];
    double *Bpart = new double[k*k];
    double *Cpart = new double[k*k];

    double *Arecv = new double[k*k];

    int *sendcounts = new int[q*q];
    int *displs = new int[q*q];

    for (int i = 0; i < q; ++i) {
        for (int j = 0; j < q; ++j) {
            sendcounts[i * q + j] = 1;
            displs[i * q + j] = (i * N * k) + (j * k);
        }
    }

    MPI_Scatterv(A, sendcounts, displs, blocktype, Apart, k*k, MPI_DOUBLE, 0, grid_comm);
    MPI_Scatterv(B, sendcounts, displs, blocktype, Bpart, k*k, MPI_DOUBLE, 0, grid_comm);

    double *Cbuf = new double [k*k];

    for (int i = 0; i < q; ++i) {
        for (int j = 0; j < k * k; j++)
        {
            Cbuf[j] = 0;
        }

        int cartRank;
        int coord[2];
        if (coordinates[1] == ((coordinates[0] + i) % q))
        {
            for (int j = 0; j < q; ++j) {
                coord[0] = coordinates[0];
                coord[1] = j;
                MPI_Cart_rank(grid_comm, coord, &cartRank);
                if (cartRank != my_grid_rank)
                {
                    MPI_Send(Apart, k * k, MPI_DOUBLE, cartRank, 1, grid_comm);
                }
                else
                {
                    for (int l = 0; l < k * k; ++l) {
                        Arecv[l] = Apart[l];
                    }
                }
            }
        }
        else
        {
            coord[0] = coordinates[0];
            coord[1] = (coordinates[0] + i) % q;
            MPI_Cart_rank(grid_comm, coord, &cartRank);
            if (cartRank != my_grid_rank)
            {
                MPI_Recv(Arecv, k * k, MPI_DOUBLE, cartRank, 1, grid_comm, &st1);
            }
        }
        for (int j = 0; j < k; j++) {
            for (int z = 0; z < k; z++) {
                for (int l = 0; l < k; l++) {
                    Cbuf[j * k + z] += Arecv[j * k + l] * Bpart[l * k + z];
                }
            }
        }
        for (int j = 0; j < k * k; j++) {
            Cpart[j] += Cbuf[j];
        }

        MPI_Sendrecv_replace(Bpart, k * k, MPI_DOUBLE, dest, 0, source, 0, grid_comm, &st2);
    }

    for (int i = 0; i < size; ++i)
        sendcounts[i] = k * k;

    MPI_Gatherv(Cpart, k * k, MPI_DOUBLE, Ccalc, sendcounts, displs, blocktype, 0, grid_comm);

    if (my_grid_rank == 0)
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

        std::cout << "RESULT IS CORRECT: " << size << " process execute in " << t2-t1 << '\n';
    }

    MPI_Finalize();
    return 0;
}