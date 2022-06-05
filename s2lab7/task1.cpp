#include <iostream>
#include <mpi.h>
#include <cmath>
#include <string>
#define eps 0.0001

// определяет размер чанка строк для каждого процесса
int get_chunk(int total, int commsize, int rank){
    int n = total;
    int q = n/commsize;
    if(n % commsize)
        q++;
    int r = commsize * q - n;
    int chunk = q;
    if(rank >= commsize - r)
        chunk = q - 1;
    return chunk;
}

// фукция формирования расширенной  матрицы
double* getExtended(int nrows, int rank, int commsize, int* rows, int n, double* a, double* aExtended, double* b)
{
    for (int i = 0; i < nrows; i++) {
        rows[i] = rank + commsize* i;
        for (int j = 0; j < n; j++)
        {
            aExtended[i * (n + 1) + j] = a[rows[i] * n + j];
        }
        aExtended[i * (n + 1) + n] = b[rows[i]];
    }
    return aExtended;
}

bool checkAbs(double curResult, double b)
{
    if(fabs((curResult-b))>0.0001)
    {
        return false;
    }
    return true;
}
int main(int argc, char *argv[]) {

    int n = 500; //Размерность СЛАУ
    int rank, commsize;
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);

    int nrows = get_chunk(n, commsize, rank); // получаем длякаждого процесса кол-во строк для работы


    int *rows = (int *) malloc(sizeof(*rows) * nrows); // Массив индексов строк СЛАУ, с которыми будет работать процесс

    // выдяляем память под
    double *a = (double *) malloc(sizeof(*a) * n*n); // Матрицу коэффициентов
    double *b = (double *) malloc(sizeof(*b) * n ); // Столбец свободных членов
    double *x = (double *) malloc(sizeof(*x) * n); // Неизвестные
    double *tmp = (double *) malloc(sizeof(*tmp) * (n + 1));

    // читаем матрицу коэф-ов и вектр свободных членов
    FILE *fpA, *fpB;
    fpA = fopen("Apcg.dat", "rb");
    fpB = fopen("Bpcg.dat", "rb");
    fread(a, sizeof(double), n * n, fpA);
    fread(b, sizeof(double), n, fpB);
    fclose(fpA);
    fclose(fpB);
    double t = MPI_Wtime();

    // Выделение памяти под расширенную матрицу
    auto *aExtended = (double *) malloc(sizeof(*a) * nrows * (n + 1));
    // Формируем расширенную матрицу
    getExtended(nrows, rank, commsize, rows, n, a, aExtended, b);


    // Первый этап решения: приводим матрицу к треугольному виду (прямой ход)
    int row = 0;
    for (int i = 0; i < n - 1; i++) {
        // на i-ом шаге исключаем xi
        if (i == rows[row]) {
            // Рассылаем i-ю строчку остальным процессам
            MPI_Bcast(&aExtended[row * (n + 1)], n + 1, MPI_DOUBLE, rank, MPI_COMM_WORLD);
            for (int j = 0; j <= n; j++)
                tmp[j] = aExtended[row * (n + 1) + j];
            row++;
        } else {
            MPI_Bcast(tmp, n + 1, MPI_DOUBLE, i % commsize, MPI_COMM_WORLD);
        }
        // вычитаем из всех остальных строчек ниже i-ю строчку чтобы привести к треугольному виду
        for (int j = row; j < nrows; j++) {
            double scaling = aExtended[j * (n + 1) + i] / tmp[i];
            for (int k = i; k < n + 1; k++)
                aExtended[j * (n + 1) + k] -= scaling * tmp[k];
        }
    }

    row = 0;
    // Заполняем неизвестные x
    for (int i = 0; i < n; i++) {
        x[i] = 0;
        if (i == rows[row]) {
            x[i] = aExtended[row * (n + 1) + n];
            row++;
        }
    }
    // Обратный ход - находим последовательно неизвестные, начиная с последей строчки в треугольнике
    row = nrows-1;
    for (int i = n -1; i > 0; i--) {
        if (row >= 0){
            if (i== rows[row]) {
                // Нашли xi, передали остальным процессам
                x[i] /= aExtended[row * (n + 1) + i];
                MPI_Bcast(&x[i], 1, MPI_DOUBLE, rank, MPI_COMM_WORLD);
                row--;
            } else

                MPI_Bcast(&x[i], 1, MPI_DOUBLE, i % commsize, MPI_COMM_WORLD);
        } else
            MPI_Bcast(&x[i], 1, MPI_DOUBLE,i % commsize, MPI_COMM_WORLD);

        for (int j = 0; j <= row; j++) // Корректировка локальных x_i
            x[rows[j]] -= aExtended[j * (n + 1) + i] * x[i];
    }
    // Корректировка x_0
    if (rank == 0)
        x[0] /= aExtended[row * (n + 1)];
    MPI_Bcast(&x[0], 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    // теперь все процессы содержат корректный вектор x[n] решений

    // проверим решение
    if (rank == 0) {
        double rs = 0;
        int correctAns = 0;
        for (int i = 0; i < n; i++) {
            // подставляем все решения и вычисляем результат подскановки.
            rs = 0;

            for (int j = 0; j < n; j++)
            {
                rs += x[j] * a[j + i * n];
            }
            // Сравниваем результат подстановки со значением вектора свободных членов
            if (std::abs(rs - b[i]) <= eps) {
                std::cout << i << " row is incorrect: " << "rs = " << rs << " b[i]: " << b[i] << '\n';
                correctAns--;
            }
            correctAns++;
        }

        if (correctAns == n)
        {
            std::cout << "Result is correct!\n";
        }

    }
    t = MPI_Wtime() - t;

    // Освобождение памяти
    free(tmp);
    free(rows);
    free(a);
    free(aExtended);
    free(b);

    if (rank == 0) {
        printf("Gaussian Elimination (MPI): n %d, procs %d, time (sec) %.6f\n", n, commsize, t);
    }
    free(x);
    MPI_Finalize();
    return 0;
}