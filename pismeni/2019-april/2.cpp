#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

#define M 4
#define N 6


int main(int argc, char* argv[])
{
	int rank, size, A[M][N], b[N], myRes[M], c[M];
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	int* myCols = (int*)malloc(M * (N / size) * sizeof(int)),
		* myVecPart = (int*)malloc((N / size) * sizeof(int));

	if (rank == 0)
	{
		printf("Matrix A:\n");
		for (int i = 0; i < M; i++)
		{
			for (int j = 0; j < N; j++)
			{
				A[i][j] = rand() % 7 + 1;
				printf("%d ", A[i][j]);
			}
			printf("\n");
		}
		printf("\n\nVector b: ");
		for (int i = 0; i < N; i++)
		{
			b[i] = rand() % 7 + 1;
			printf("%d ", b[i]);
		}
		printf("\n\n");
	}

	MPI_Datatype cols, cols_resized, vec, vec_resized, recv;
	int* lengths = (int*)malloc(M * (N / size) * sizeof(int)),
		* displacements = (int*)malloc(M * (N / size) * sizeof(int));
	for (int i = 0; i < (M * (N / size)); i++)
	{
		lengths[i] = 1;
		displacements[i] = i * N / size;
	}
	MPI_Type_indexed(M * (N / size), lengths, displacements, MPI_INT, &cols);
	MPI_Type_commit(&cols);
	MPI_Type_create_resized(cols, 0, sizeof(int), &cols_resized);
	MPI_Type_commit(&cols_resized);

	MPI_Type_vector(N / size, 1, size, MPI_INT, &vec);
	MPI_Type_commit(&vec);
	MPI_Type_create_resized(vec, 0, sizeof(int), &vec_resized);
	MPI_Type_commit(&vec_resized);

	/*MPI_Type_vector(M, 1, N / size, MPI_INT, &recv);
	MPI_Type_commit(&recv);
	MPI_Type_create_resized(recv, 0, sizeof(int), &recv_resized);
	MPI_Type_commit(&recv_resized);*/

	MPI_Scatter(&A[0][0], 1, cols_resized, myCols, M * N / size, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatter(b, 1, vec_resized, myVecPart, (N / size), MPI_INT, 0, MPI_COMM_WORLD);

	/*printf("Process %d. My cols:\n", rank);
	for (int i = 0; i < M; i++)
	{
		for (int j = 0; j < N / size; j++)
			printf("%d ", myCols[i * (N / size) + j]);
		printf("\n");
	}
	printf("\nProcess %d. My vec part: ", rank);
	for (int i = 0; i < N / size; i++)
		printf("%d ", myVecPart[i]);
	printf("\n\n");*/


	for (int i = 0; i < M; i++)
	{
		int res = 0;
		for (int j = 0; j < N / size; j++)
		{
			res += myCols[i * (N / size) + j] * myVecPart[j];
		}
		myRes[i] = res;
	}

	MPI_Reduce(myRes, c, M, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	if (rank == 0)
	{
		printf("Vector C: ");
		for (int i = 0; i < M; i++)
			printf("%d ", c[i]);
		printf("\n");
	}

	MPI_Finalize();
	free(myCols);
	free(myVecPart);
	free(displacements);
	free(lengths);
}

