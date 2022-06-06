#include <iostream>
#include <omp.h>
#include <mpi.h>
#define N 100
const double EPS = 1e-7;


void conGrad(double* A, double* B, double *x, double prevGrad, double nextGrad, double* grad, double* direction, double offset)
{
    prevGrad = 0.0;

    for (int i = 0; i < N; i++)
    {
        prevGrad += grad[i] * grad[i];
    }

    for (int i = 0; i < N; i++)
    {
        double res = 0;
        for (int j = 0; j < N; j++)
        {
            res += A[j + i * N] * x[j];
        }
        grad[i] = B[i] - res;
    }
    nextGrad = 0.0;

    for (int i = 0; i < N; i++)
    {
        nextGrad += grad[i] * grad[i];
    }

    for (int i = 0; i < N; i++)
    {
        direction[i] = ((nextGrad / prevGrad) * direction[i]) - grad[i];
    }

    double offsetNum = 0.0;
    for (int i = 0; i < N; i++)
    {
        offsetNum += direction[i] * grad[i];
    }

    double offsetDen = 0.0;
    for (int i = 0; i < N; i++)
    {
        double partDirMatrixScalar = 0.0;
        for (int j = 0; j < N; j++)
        {
            partDirMatrixScalar += direction[j] * A[j + i * N];
        }

        offsetDen += partDirMatrixScalar * direction[i];
    }
    offset = offsetNum / offsetDen;
    for (int i = 0; i < N; i++)
    {
        x[i] += offset * direction[i];
    }
}

bool isPrecised(const double *A, const double *B, const double *x)
{

    double result[N];

    for (double & i : result)
    {
        i = 0;
    }

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; ++j)
        {
            result[i] += A[j + (i * N)] * x[j];
        }
    }

    for (int i = 0; i < N; ++i) {
        result[i] = result[i] - B[i];
    }
    for (double i : result)
    {
        if (std::abs(i)>EPS)
        {
            return false;
        }
    }
    return true;
}

int main() {
    double *A = (double *) malloc(sizeof(*A) * N * N);
    double *B = (double *) malloc(sizeof(*B) * N);
    double t1,t2;

    FILE *fpA, *fpB;

    fpA = fopen("Apcg.dat", "rb");
    fpB = fopen("Bpcg.dat", "rb");

    fread(A, sizeof(double), N * N, fpA);
    fread(B, sizeof(double), N, fpB);
    fclose(fpA);
    fclose(fpB);


    double grad[N];
    for (int i = 0; i < N; i++)
    {
        grad[i] = B[i];
    }

    double direction[N] = {0};
    double x[N] = {0};
    double prevGrad = 0, nextGrad = 0, offset = 0;


    t1 = MPI_Wtime();
    do {
        conGrad(A, B, x, prevGrad, nextGrad, grad, direction, offset);
    } while (!isPrecised(A, B, x));


    t2 = MPI_Wtime() - t1;


    for (int i = 0; i < 10; i++){
        for (int j = 0; j < 10; j++)
        {
            std::cout << "x[" << i*10 + j << "]: " << x[i] << "\t\t";
        }
        std::cout << '\n';
    }
    std::cout << "Время работы: " << t2 <<"\n\n";

    return 0;
}
