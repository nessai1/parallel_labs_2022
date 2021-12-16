#include <mpi.h>
#include <stdio.h>
#include <algorithm>
#include <iostream>

#define NPROCS 8

int main(int argc, char* argv[]){
    int rank, new_rank, sendbuf, recvbuf, numtasks, ranks1[4] = {0,1,2,3}, ranks2[4] = {4,5,6,7};
    MPI_Group orig_group, new_group_min, new_group_max;
    MPI_Comm new_comm_min, new_comm_max;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

    if (numtasks != NPROCS){
        printf("Must specify MP_PROCS = %d. Terminating. \n", NPROCS);
        MPI_Finalize();
        return 0;
    }

    sendbuf = rank;

    MPI_Comm_group(MPI_COMM_WORLD, &orig_group);

    if (rank < NPROCS/2)
    {
        MPI_Group_incl(orig_group, NPROCS/2, ranks1, &new_group_min);
        MPI_Comm_create(MPI_COMM_WORLD, new_group_min, &new_comm_min);
    }
    else
    {
        MPI_Group_incl(orig_group, NPROCS/2, ranks2, &new_group_max);
        MPI_Comm_create(MPI_COMM_WORLD, new_group_max, &new_comm_max);
    }

    if (std::find(ranks1, ranks1+4,rank) != (ranks1+4))
    {
        MPI_Group_rank(new_group_min, &new_rank);
        int minrank = 0;
        int newMinrank;
        MPI_Group_translate_ranks(orig_group, 1, &minrank, new_group_min, &newMinrank);
        MPI_Reduce(&sendbuf, &recvbuf, 1, MPI_INT, MPI_MIN, newMinrank, new_comm_min);
        if (new_rank == newMinrank)
        {
            std::cout << "rank = " << rank << " newrank = " << new_rank << " recvbuf = " << recvbuf << '\n';
        }
    }
    else if (std::find(ranks2, ranks2+4,rank) != (ranks2+4))
    {
        MPI_Group_rank(new_group_max, &new_rank);
        int maxrank = NPROCS - 1;
        int newMaxrank;
        MPI_Group_translate_ranks(orig_group, 1, &maxrank, new_group_max, &newMaxrank);
        MPI_Reduce(&sendbuf, &recvbuf, 1, MPI_INT, MPI_MAX, newMaxrank, new_comm_max);
        if (new_rank == newMaxrank)
        {
            std::cout << "rank = " << rank << " newrank = " << new_rank << " recvbuf = " << recvbuf << '\n';
        }
    }

    MPI_Finalize();
}
