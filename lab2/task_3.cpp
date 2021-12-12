#include <iostream>
#include <omp.h>
#include <vector>

int main()
{
    int N;
    std::cout << "Enter N: ";
    std::cin >> N;

    std::vector<std::vector<int>> A(N), B(N), C(N);
    for (int i = 0; i < N; i++)
    {
        A[i] = std::vector<int>(N, 0);
        B[i] = std::vector<int>(N, 0);
        C[i] = std::vector<int>(N, 0);
    }

    std::cout << "Enter matrix A\n-----\n";
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            std::cin >> A[i][j];
        }
    }

    std::cout << "Enter matrix B\n-----\n";
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            std::cin >> B[i][j];
        }
    }

    int threads, a;
    std::cout << "Enter threads: ";
    std::cin >> threads;

    omp_set_num_threads(threads);
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            a = 0;
            #pragma omp parallel for reduction(+:a)
            for (int q = 0; q < N; q++)
            {
                a += A[i][q] * B[q][j];
            }
            C[i][j] = a;
        }
    }

    std::cout << "result: \n";
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            std::cout << C[i][j] << ' ';
        }
        std::cout << '\n';
    }

    return 0;
}