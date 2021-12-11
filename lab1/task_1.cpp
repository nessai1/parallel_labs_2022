#include <iostream>
#include <omp.h>

int main() {
    int threads = 5;
    omp_set_num_threads(threads); // Указали количество нитей = threads = 5

    #pragma omp parallel
    {
        #pragma omp critical
        {
            std::cout << " I'm " << omp_get_thread_num() << " thread from " << omp_get_num_threads()  << " threads!\n";
        }
    };
}
