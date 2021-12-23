#include <iostream>
#include <omp.h>
#define THREADS 4

double a1,a2,a3,a,b;
int N;

double f(double x)
{
    return (a1*x*x) + (a2*x) + a3;
}

int main() {
    std::cout << "Enter N: ";
    std::cin >> N;

    std::cout << "Enter a1 a2 a3: ";
    std::cin >> a1 >> a2 >> a3;

    std::cout << "Enter a b: ";
    std::cin >> a >> b;

    omp_set_num_threads(THREADS);

    double h = (a+b)/N;
    double sum = 0;
    double rs = 0;
    #pragma omp parallel reduction(+ : rs) private(sum)
    {
        #pragma omp  for
        for (int i = 0; i < N; i++)
        {
            double x = h * ((double)i + 0.5);
            sum += f(x);
        }
        rs = h * sum;
    }

    std::cout << "RESULT: " << rs << '\n';
}
