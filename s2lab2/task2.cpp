#include <iostream>
#include <ctime>
#include <omp.h>

#define MAX_VALUE 100

int main(int argc, char** argv) {
    srand(time(nullptr));

    int n, size;

    std::cout << "Enter number of threads: ";
    std::cin >> size;

    do {
        std::cout << "Enter N: ";
        std::cin >> n;
        if (n < size || n % size != 0) {
            std::cout << "N >= p-size and N mod p-size = 0\n";
        }
    } while (n < size || n % size != 0);

    int *A, *V, *rsA, *Vrs;
    A = new int[n*n];
    V = new int [n];
    //rsA = new int[n*n];
    Vrs = new int[n];

    for (int i = 0; i < n*n; i++)
    {
        A[i] = rand() % MAX_VALUE;
    }
    for (int i = 0; i < n; i++)
    {
        V[i] = rand() % MAX_VALUE;
    }

    for (int i = 0; i < n; i++)
    {
        Vrs[i] = 0;
    }

    std::cout << "\n\n\tGenerated matrix\n";
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            std::cout << A[i * n + j] << ' ';
        }
        std::cout << '\n';
    }

    std::cout << "\n\n\tGenerated Vector\n";
    for (int i = 0; i < n; i++)
    {
        std::cout << V[i] << '\n';
    }
    std::cout << "--------------------------\n";




    #pragma omp parallel num_threads(size)
    {
        int threadNum = omp_get_thread_num();

        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < (n/size); j++)
            {
                #pragma omp critical
                Vrs[i] += A[i * n + (j + (n/size) * threadNum)] * V[j + (n/size) * threadNum];
            }
        }
    }

    std::cout << "result Matrix: \n";
    for (int i = 0; i < n; i++)
    {
        std::cout << Vrs[i] << '\n';
    }
    return 0;
}
