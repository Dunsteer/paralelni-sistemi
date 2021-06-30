#include <stdio.h>
#include <math.h>
#include "mpi.h"

void main(int* argc, char* argv[])
{
	int size, rank;
	int x = 0;
	int src, dst;
	int coords[2];
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Status status;

	int dims[] = { 4,4 };
	int periods[] = { 0,1 };
	MPI_Comm car;

	MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &car);
	MPI_Cart_coords(car, rank, 2, coords);
	x = coords[0] + coords[1];
	MPI_Cart_shift(car, 1, -coords[0], &src, &dst);
	MPI_Sendrecv_replace(&x, 1, MPI_INT, dst, 0, src, 0, car, &status);
}