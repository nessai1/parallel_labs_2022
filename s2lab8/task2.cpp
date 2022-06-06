#include <iostream>
#include <omp.h>
#include <mpi.h>
#define N 100
const double EPS = 1e-7;


void conGrad(double* A, double* B, double *x, double prevGrad, double nextGrad, double* grad, double* grad_part, double* direction, double* direction_part, double offset, double offsetDen, double offsetNum)
{
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0)
    {
        prevGrad = 0;
        for (int i = 0; i < N; i++)
        {
            prevGrad += grad[i] * grad[i];
        }
    }
    double result_part[N];
    double Bpart[N / size];
    for (int i = 0; i < N; i++)
    {
        grad_part[i] = 0;
        result_part[i] = 0;
    }
    MPI_Scatter(x, N / size, MPI_DOUBLE, Bpart, N / size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    for (int i = 0; i < N / size; i++)
    {
        for (int j = 0; j < N; j++)
        {
            result_part[j] += A[j + i * N] * Bpart[i];
        }
    }
    MPI_Reduce(result_part, grad_part, N, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    if (rank == 0)
    {
        for (int i = 0; i < N; i++)
        {
            grad[i] = B[i] - grad_part[i];
        }
        nextGrad = 0;
        for (int i = 0; i < N; i++)
        {
            nextGrad += grad[i] * grad[i];
        }
        for (int i = 0; i < N; i++)
        {
            direction[i] = (nextGrad / prevGrad) * direction[i] - grad[i];
        }
    }

    for (int i = 0; i < N; ++i) {
        direction_part[i] = 0;
        result_part[i] = 0;
    }
    MPI_Scatter(direction, N / size, MPI_DOUBLE, Bpart, N / size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    for (int i = 0; i < N / size; i++)
    {
        for (int j = 0; j < N; j++)
        {
            result_part[j] += A[j + i * N] * Bpart[i];
        }
    }
    MPI_Reduce(result_part, direction_part, N, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    if (rank == 0)
    {
        offsetDen = 0.0;
        offsetNum = 0.0;
        for (int i = 0; i < N; i++)
        {
            offsetDen += direction_part[i] * direction[i];
            offsetNum += direction[i] * grad[i];
        }
        offset = offsetNum / offsetDen;
        for (int i = 0; i < N; i++)
        {
            x[i] += offset * direction[i];
        }
    }
}

bool isPrecised(const double *A, const double *B, const double *x)
{
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double result[N];
    bool precised = true;

    double result_part[N];
    double Bpart[N / size];
    for (int i = 0; i < N; ++i)
    {
        result[i] = 0;
        result_part[i] = 0;
    }
    MPI_Scatter(x, N / size, MPI_DOUBLE, Bpart, N / size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    for (int i = 0; i < N / size; i++)
    {
        for (int j = 0; j < N; j++)
        {
            result_part[j] += A[j + i * N] * Bpart[i];
        }
    }
    MPI_Reduce(result_part, result, N, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    if (rank == 0)
    {
        for (int i = 0; i < N; i++)
        {
            result[i] = result[i] - B[i];
        }
        for (int i = 0; i < N; i++)
        {
            if (std::abs(result[i]) > EPS)
            {
                precised = false;
                MPI_Bcast(&precised, 1, MPI_C_BOOL, 0, MPI_COMM_WORLD);
                return precised;
            }
        }
        precised = true;
        MPI_Bcast(&precised, 1, MPI_C_BOOL, 0, MPI_COMM_WORLD);
        return precised;
    }

    MPI_Bcast(&precised, 1, MPI_C_BOOL, 0, MPI_COMM_WORLD);
    return precised;
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int partSize = N*N / size;

    double *A = (double *) malloc(sizeof(*A) * N * N);
    double *At = (double *) malloc(sizeof(*At) * N * N);
    double *B = (double *) malloc(sizeof(*B) * N);
    double t1,t2;

    if (rank == 0)
    {
        FILE *fpA, *fpB;

        fpA = fopen("Apcg.dat", "rb");
        fpB = fopen("Bpcg.dat", "rb");

        fread(A, sizeof(double), N * N, fpA);
        fread(B, sizeof(double), N, fpB);
        fclose(fpA);
        fclose(fpB);
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                At[j + (i * N)] = A[i + (j * N)];
            }
        }
    }

    double *At_part = new double[partSize];
    MPI_Scatter(At, partSize, MPI_DOUBLE, At_part, partSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    double grad[N];
    double grad_part[N];
    for (int i = 0; i < N; i++)
    {
        grad[i] = -B[i];
    }

    double direction[N] = {0};
    double x[N] = {0};
    double prevGrad = 0, nextGrad = 0, offset = 0, offsetDen = 0, offsetNum = 0;

    double direction_part[N];

    t1 = MPI_Wtime();
    do {
        conGrad(At_part, B, x, prevGrad, nextGrad, grad, grad_part, direction, direction_part, offset, offsetDen, offsetNum);
    } while (!isPrecised(At_part, B, x));


    t2 = MPI_Wtime() - t1;

    if (rank == 0)
    {
        for (int i = 0; i < 10; i++){
            for (int j = 0; j < 10; j++)
            {
                std::cout << "x[" << i*10 + j << "]: " << x[i*10 + j] << "\t\t";
            }
            std::cout << '\n';
        }
        std::cout << "Время работы: " << t2 <<"\n\n";
    }


    MPI_Finalize();
    return 0;
}
