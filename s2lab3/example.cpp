#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define COLS  12 /* количество столбцов*/
#define ROWS  8 /* количество строк*/

int main(int argc, char **argv) {

    MPI_Init(&argc, &argv);
    int p, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    char i;

    char a[ROWS*COLS];
    const int NPROWS=2;  /* количество строк в разбиение (количество блоков по первой размерности) */
    const int NPCOLS=3;  /* количество столбцов в разбиении (количество блоков по второй размерности) */
    const int BLOCKROWS = ROWS/NPROWS;  /* количество строк в блоке */
    const int BLOCKCOLS = COLS/NPCOLS; /* количество столбцов в блоке */

    if (rank == 0) {
        for (int ii=0; ii<ROWS*COLS; ii++) {
            a[ii] = (char)ii;
        }
    }

    if (p != NPROWS*NPCOLS) {
        fprintf(stderr,"Error: number of PEs %d != %d x %d\n", p, NPROWS, NPCOLS);
        MPI_Finalize();
        exit(-1);
    }
    char b[BLOCKROWS*BLOCKCOLS];
    for (int ii=0; ii<BLOCKROWS*BLOCKCOLS; ii++) b[ii] = 0;
/* создаем тип, описывающий разбиение матрицы на блоки */
    MPI_Datatype blocktype;
    MPI_Datatype blocktype2;
/* описываем производный тип BLOCKROWS - количество блоков, BLOCKCOLS - длина блока, COLS - количество элементов, расположенных между началом предыдущего и началом следующего блока*/
    MPI_Type_vector(BLOCKROWS, BLOCKCOLS, COLS, MPI_CHAR, &blocktype2);

/* промежуточная процедура, позволяющая использовать процедуру MPI_Scatter для вновь созданного типа,
https://www.opennet.ru/docs/RUS/mpi-2/node73.html*/

    MPI_Type_create_resized( blocktype2, 0, sizeof(char), &blocktype);

/* фиксируем тип*/
    MPI_Type_commit(&blocktype);

/* определяем расстояния между блоками и размер каждого блока*/
    int disps[NPROWS*NPCOLS];
    int counts[NPROWS*NPCOLS];
    for (int ii=0; ii<NPROWS; ii++) {
        for (int jj=0; jj<NPCOLS; jj++) {
            disps[ii*NPCOLS+jj] = ii*COLS*BLOCKROWS+jj*BLOCKCOLS;
            counts [ii*NPCOLS+jj] = 1;
        }
    }
/* используем MPI_Scatterv для передачи блоков матрицы A*/
    MPI_Scatterv(a, counts, disps, blocktype, b, BLOCKROWS*BLOCKCOLS, MPI_CHAR, 0, MPI_COMM_WORLD);


    /* each proc prints it's "b" out, in order */
    for (int proc=0; proc<p; proc++) {
        if (proc == rank) {
            printf("Rank = %d\n", rank);
            if (rank == 0) {
                printf("Global matrix: \n");
                for (int ii=0; ii<ROWS; ii++) {
                    for (int jj=0; jj<COLS; jj++) {
                        printf("%3d ",(int)a[ii*COLS+jj]);
                    }
                    printf("\n");
                }
            }
            printf("Local Matrix:\n");
            for (int ii=0; ii<BLOCKROWS; ii++) {
                for (int jj=0; jj<BLOCKCOLS; jj++) {
                    printf("%3d ",(int)b[ii*BLOCKCOLS+jj]);
                }
                printf("\n");
            }
            printf("\n");
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Finalize();

    return 0;
}
