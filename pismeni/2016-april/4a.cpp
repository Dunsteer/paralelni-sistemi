#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    int size, rank;
    double width;
    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(rank == 0)
        width = 1.0 / size;

    MPI_Bcast(&width, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    double myX = (rank + 0.5) * width;
    double myF = 4.0 / (1 + myX * myX);
    double myArea = myF * width;

    double pi;
    MPI_Reduce(&myArea, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
        printf("Pi: %lf.\n", pi);

    MPI_Finalize();
}

