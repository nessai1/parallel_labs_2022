#include <iostream>
#include <omp.h>

int main() {
    int k;
    std::cout << "Enter number of threads (k): ";
    std::cin >> k;

    omp_set_num_threads(k);
    #pragma omp parallel
    {
        #pragma omp sections
        {
            #pragma omp section
            {
                #pragma omp critical
                std::cout << "[" << omp_get_thread_num() << "]: came in section "<< 1 << '\n';
            }
            #pragma omp section
            {
                #pragma omp critical
                std::cout << "[" << omp_get_thread_num() << "]: came in section "<< 2 << '\n';
            }
            #pragma omp section
            {
                #pragma omp critical
                std::cout << "[" << omp_get_thread_num() << "]: came in section "<< 3 << '\n';
            }
        }
        #pragma omp critical
        std::cout << "[" << omp_get_thread_num() << "]: parallel region\n";
    };
    return 0;
}
