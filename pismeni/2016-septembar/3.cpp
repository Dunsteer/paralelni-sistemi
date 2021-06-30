#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

#define N 4
#define K 2

int main(int argc, char* argv[])
{
    int A[N][N], B[N][N], rank, myRows[N / K][N], myColumns[N][N / K];
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
    {
        printf("A:\n");
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                A[i][j] = rand() % 6 + 1;
                printf("%d ", A[i][j]);
            }
            printf("\n");
        }
        printf("\n");

        printf("B:\n");
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                B[i][j] = rand() % 6 + 1;
                printf("%d ", B[i][j]);
            }
            printf("\n");
        }
        printf("\n");
    }


    MPI_Datatype Column, Column_resized, Row;
    MPI_Type_vector(N, N/K, N, MPI_INT, &Column);
    MPI_Type_commit(&Column);
    MPI_Type_create_resized(Column, 0, (N/K) * sizeof(int), &Column_resized);
    MPI_Type_commit(&Column_resized);
    MPI_Type_vector(N/K, N, N, MPI_INT, &Row);
    MPI_Type_commit(&Row);

    MPI_Scatter(&A[0][0], 1, Column_resized, myColumns, N * (N / K), MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(&B[0][0], 1, Row, myRows, N * (N / K), MPI_INT, 0, MPI_COMM_WORLD);

    int myResPart[N][N];
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            int res = 0;
            for (int k = 0; k < N / K; k++)
            {
                res += myColumns[i][k] * myRows[k][j];
            }
            myResPart[i][j] = res;
        }
    }

    fflush(stdout);
    MPI_Barrier(MPI_COMM_WORLD);
    printf("Process %d res part:\n", rank);
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
            printf("%d ", myResPart[i][j]);
        printf("\n");
    }
    printf("\n");
    fflush(stdout);
    MPI_Barrier(MPI_COMM_WORLD);

    int C[N][N];

    MPI_Reduce(&myResPart[0][0], *C, N * N, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    
    if (rank == 0)
    {
        printf("Final result: \n");
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                printf("%d ", C[i][j]);
            }
            printf("\n");
        }
    }

    MPI_Finalize();
}

