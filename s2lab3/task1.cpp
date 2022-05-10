#include <mpi.h>
#include <omp.h>
#include <ctime>
#include <iostream>
#include <cmath>
#define MAX_VALUE 100


int main(int argc, char** argv) {
    setlocale(LC_ALL, "ru");
    MPI_Init(&argc, &argv);
    srand(time(nullptr));

    int N, size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);


    double bf;
    double ost = modf(std::sqrt(size), &bf);
    if (ost > 0)
    {
        if (rank == 0)
        {
            std::cout << "[ERROR]: Корень количества процессов не должен иметь дробную часть\n";
        }
        MPI_Finalize();
        exit(MPI_ERR_ARG);
    }

    int s = std::sqrt(size);

    int m;
    if (rank == 0)
    {
        do
        {
            std::cout << "Введите размерность матрицы N: ";
            std::cin >> N;
            ost = modf(static_cast<double>(N)/static_cast<double>(s), &bf);
            if (ost > 0)
            {
                std::cout << "N должен быть кратен s (" << s << ")\n";
            }
            else
            {
                break;
            }
        } while (true);
    }

    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
    int *A, *V, *Arecv, *Vcalc, *Vrs;
    A = new int[N*N];
    Arecv = new int[N*N];
    V = new int[N];
    Vcalc = new int[N];
    Vrs = new int[N];

    for (int i = 0; i < N; i++)
    {
        Vcalc[i] = 0;
    }

    m = N/s;
    if (rank == 0)
    {
        std::cout << "N: " << N << " m: " << m << " s: " << s << " p: " << size << '\n';
        for (int i = 0; i < N*N; i++)
        {
            A[i] = rand() % MAX_VALUE;
        }
        for (int i = 0; i < N; i++)
        {
            V[i] = rand() % MAX_VALUE;
        }


        std::cout << "Generated matrix\n";


        /*
         * здесь мы выводим нашу матрицу, параллельно демонстрируя наше блочное устройство
         * Матрица представлена в виде одномерного массива.
         * первые m*m элементов - наш первый блок
         * вторые m*m элементов - наш второй блок
         * и.т.д.
         * m*m*s*i + m*m*j  - указатель на начало блока (i,j)
         */
        for (int i = 0; i < s; i++) // проходимся по каждой строчке с блоками, их у нас s
        {
            /* для того, что-бы вывести каждую строчку для блока в строчке i и не поломать порядок, проходимся m раз по строке i */
            for (int q = 0; q < m; q++)
            {
                /* выводим одну строчку блока (i,j) */
                for (int j = 0; j < s; j++)
                {
                    for (int k = 0; k < m; k++)
                    {
                        std::cout << A[m*m*s*i + j*m*m + q*m + k] << '\t';
                    }
                }
                std::cout << '\n';
            }
        }

        std::cout << "Generated vector\n";
        for (int i = 0; i < N; i++)
        {
            std::cout << V[i] << '\n';
        }

        std::cout << "--------------------";
        /*
            Пример вывода блока (2;2)

            std::cout << "(2;2):\n";
            for (int i = 0; i < m; i++)
            {
                for (int j = 0; j < m; j++)
                {
                    std::cout << A[m*m*s*1 + m*m*1 + i*m + j] << ' ';
                }
                std::cout << '\n';
            }
         */
    }

    MPI_Bcast(V, N, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(A, m*m, MPI_INT, Arecv, m*m, MPI_INT, 0, MPI_COMM_WORLD);
    int block_i = rank / s, block_j = rank % s;
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < m; j++)
        {
            Vcalc[block_i*m + i] += Arecv[i*m + j] * V[block_j*m + j];
        }
    }

    MPI_Reduce(Vcalc, Vrs, N, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    if (rank == 0)
    {
        std::cout << "\nResult vector:\n";
        for (int i = 0; i < N; i++)
        {
            std::cout << Vrs[i] << '\n';
        }
    }

    MPI_Finalize();

    return 0;
}
