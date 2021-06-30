#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

#define N 5

int main(int argc, char* argv[])
{
    int S[N][N], rank, size, myRow[N], rowSum = 0, fullSum, b[N], myColumn[N], myElement, C[N];
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0)
    {
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                S[i][j] = rand() % 4 + 1;
                printf("%d ", S[i][j]);
            }
            printf("\n");
        }
    }

    MPI_Scatter(&S[0][0], N, MPI_INT, myRow, N, MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = 0; i < N; i++)
        rowSum += myRow[i];

    printf("Process: %d. My row sum is: %d.\n\n", rank, rowSum);
    fflush(stdout);

    MPI_Reduce(&rowSum, &fullSum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        printf("Total matrix sum is: %d.\n\n", fullSum);
        fflush(stdout);
    }

    struct { int val; int rank; } in, out;
    in.val = rowSum;
    in.rank = rank;

    MPI_Reduce(&in, &out, 1, MPI_2INT, MPI_MAXLOC, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    int maxRank = out.rank;
    if (rank == 0)
        printf("\n\nThe process with the largest sum is: %d.\n\n", maxRank);

    MPI_Bcast(&maxRank, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int A[N][N];
    if (rank == maxRank)
    {
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                A[i][j] = rank;
    }

    MPI_Gather(&rowSum, 1, MPI_INT, &A[2][0], 1, MPI_INT, maxRank, MPI_COMM_WORLD);

    if (rank == maxRank)
    {
        printf("\nMatrix A:\n");
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
                printf("%d ", A[i][j]);
            printf("\n");
        }
        printf("\n");

        printf("Vector b:\n");
        for (int i = 0; i < N; i++)
        {
            b[i] = rand() % 7 + 1;
            printf("%d ", b[i]);
        }
        printf("\n");
    }

    MPI_Datatype column, columnResized;
    MPI_Type_vector(N, 1, N, MPI_INT, &column);
    MPI_Type_commit(&column);

    MPI_Type_create_resized(column, 0, sizeof(int), &columnResized);
    MPI_Type_commit(&columnResized);

    MPI_Scatter(&A[0][0], 1, columnResized, myColumn, N, MPI_INT, maxRank, MPI_COMM_WORLD);
    MPI_Scatter(b, 1, MPI_INT, &myElement, 1, MPI_INT, maxRank, MPI_COMM_WORLD);

    int myResPart[N];

    for (int i = 0; i < N; i++)
        myResPart[i] = myColumn[i] * myElement;

    MPI_Reduce(myResPart, C, N, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        printf("\Vector C:\n");
        for (int i = 0; i < N; i++)
        {
            printf("%d ", C[i]);
        }
        printf("\n\n");
    }

    MPI_Finalize();
}

