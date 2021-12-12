#include <iostream>
#include <omp.h>

int main()
{
    int k,N;

    std::cout << "Enter number of threads (k): ";
    std::cin >> k;
    std::cout << "Enter N: ";
    std::cin >> N;

    omp_set_num_threads(k);

    int sum = 0;
    #pragma omp parallel reduction(+:sum)
    {
        int chunk = 0;
        #pragma omp for schedule(static)
        for (int i = 1; i <= N; i++)
        {
            #pragma omp critical
            std::cout << '[' << omp_get_thread_num() << "]: calculation of the iteration number " << i-1 << '\n';
            chunk += i;
        }

        sum += chunk;

        #pragma omp critical
        std::cout << '[' << omp_get_thread_num() << "]: Sum = " << chunk << '\n';
    }

    std::cout << "Sum = " << sum << '\n';
}
