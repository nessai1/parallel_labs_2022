#include <iostream>
#include <vector>
#include <algorithm>
#include <omp.h>

int main() {
    std::vector<int> arr;
    int k, rank;

    std::cout << "Enter threads number: ";
    std::cin >> k;
    omp_set_num_threads(k); // Указали количество нитей = threads

    #pragma omp parallel shared(arr) private(rank)
    {
        rank = omp_get_thread_num();
        if (rank % 2 == 1)
        {
            #pragma omp critical
            {
                arr.push_back(rank);
                std::cout << " I'm " << rank << " thread\n";
            }
        }
    };

    // Алгоритм для определения дубликатов
    std::sort(arr.begin(), arr.end());
    for (int i = 0; i < arr.size()-1; i++)
    {
        if (arr[i] == arr[i+1])
        {
            std::cout << arr[i] << " duplicated!\n";
        }
    }
}
