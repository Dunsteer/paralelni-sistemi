#include <stdio.h>
#include <math.h>
#include "mpi.h"
#define m 8

void main(int* argc, char* argv[])
{
	int A[m][m];
	int B[m];
	int p;
	int q;
	int rank, i, j;
	MPI_Datatype subMatrica;
	MPI_Datatype vektor;
	MPI_Status status;
	int offsetI, offsetJ; 

	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	q = sqrt(p);
	MPI_Type_vector(q+1, q+1, m, MPI_INT, &subMatrica);
	MPI_Type_commit(&subMatrica);

	MPI_Type_vector(1, q + 1, 0, MPI_INT, &vektor);
	MPI_Type_commit(&vektor);

	if (rank == 0) {
		for (i = 0; i < m; i++) {
			for (j = 0; j < m; j++) {
				A[i][j] = rand() % 100;
			}
		}
		for (i = 0; i < m; i++)
			B[i] = rand() % 100;

		for (i = 1; i < p; i++) {
			offsetI = i / q;
			offsetJ = i % q;
			MPI_Send(&A[offsetI][offsetJ], 1, subMatrica, i, 0, MPI_COMM_WORLD);

			MPI_Send(&B[offsetI], 1, vektor, i, 0, MPI_COMM_WORLD);
		}
	}
	else {
		MPI_Recv(&A[0][0], 1, subMatrica, 0, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(&B[0], 1, vektor, 0, 0, MPI_COMM_WORLD, &status);
	}

	//mnozenje matrica

	//trazenje max





}