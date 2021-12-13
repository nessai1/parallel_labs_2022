#include <iostream>
#include <omp.h>
#include <vector>
#include <cmath>
#include <chrono>
#include <ctime>

int main()
{
    srand(time(0));
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

    std::cout << "Randomized matrix A and B (" << N << 'x' << N << ")...\n";
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            A[i][j] = 1 + rand() % 1000;
            B[i][j] = 1 + rand() % 1000;
        }
    }

    int threads, a;
    std::cout << "Enter threads: ";
    std::cin >> threads;

    omp_set_num_threads(threads);
    auto start = std::chrono::high_resolution_clock::now();
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
    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "Parallel Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count() << "ns" << '\n';

    //// without OpenMP
    for (int i = 0; i < N; i++)
    {
        C[i] = std::vector<int>(N, 0);
    }

    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            a = 0;
            for (int q = 0; q < N; q++)
            {
                a += A[i][q] * B[q][j];
            }
            C[i][j] = a;
        }
    }
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Single thread Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ns"  << '\n';
    return 0;
}