#include <omp.h>
#include <iostream>
#include <vector>

int main()
{
    int k, N;
    std::cout << "Enter number of threads: ";
    std::cin >> k;

    std::cout << "Enter N: ";
    std::cin >> N;
    std::vector A(N+1, 0);

    omp_set_num_threads(k);
    #pragma omp parallel shared(A)
    {
        #pragma omp for schedule(static) //nowait
        for (int i = 1; i <= N; i++)
        {
            A[i] = i*i;
            #pragma omp critical
            {
                std::cout << "[" << omp_get_thread_num() << "]: calculation of the iteration number " << i-1 << ".\n";
                std::cout << "A[" << i << "]: " << A[i] << '\n';
            }
        }

        #pragma omp barrier

        #pragma omp master
        {
            for (int i = 1; i <= N; i++)
            {
                #pragma omp critical
                std::cout << "MASTER: A[" << i << "]: " << A[i] << '\n';
            }
        }
    }

    return 0;
}