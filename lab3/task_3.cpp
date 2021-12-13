#include <iostream>
#include <cstdlib>
#include <omp.h>
using namespace std;
int main() {
    double t = omp_get_wtime();
    int sum = 0;

    omp_set_num_threads(4);
    #pragma omp parallel
    {
        #pragma omp sections reduction(+:sum)
        {
            #pragma omp section
            {
                for (int i = 0; i < 500000; i++) {
                    sum++;
                }
            }

            #pragma omp section
            {
                for (int i = 0; i < 500000; i++) {
                    sum++;
                }
            }

            #pragma omp section
            {
                for (int i = 0; i < 500000; i++) {
                    sum++;
                }
            }

            #pragma omp section
            {
                for (int i = 0; i < 500000; i++) {
                    sum++;
                }
            }
        }
    };
    cout << "Sum: " << sum << endl;
    cout << "Time: " << omp_get_wtime() - t << endl;

    return EXIT_SUCCESS;
}