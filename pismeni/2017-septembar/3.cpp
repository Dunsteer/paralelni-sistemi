#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

#define N 6
#define M 4
#define K 3

int main(int argc, char* argv[])
{
    int rank, size, A[M][N], B[N][K], C[M][K], myRes[M][K];
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int* myCols = (int*)malloc((N / size) * M * sizeof(int)),
        * myRows = (int*)malloc((N / size) * K * sizeof(int));

    int p = N / size;

    if (rank == 0)
    {
        printf("Matrix A:\n");
        for (int i = 0; i < M; i++)
        {
            for (int j = 0; j < N; j++)
            {
                A[i][j] = rand() % 55 + 1;
                printf("%d ", A[i][j]);
            }
            printf("\n");
        }
        printf("\n\n");

        printf("Matrix B:\n");
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < K; j++)
            {
                B[i][j] = rand() % 20 + 1;
                printf("%d ", B[i][j]);
            }
            printf("\n");
        }
        printf("\n\n");
    }

    MPI_Datatype cols, cols_resized, rows, rows_resized;
    int numBlocks = M * (N / size);
    int* lengths = (int*)malloc(numBlocks * sizeof(int)), * displacements = (int*)malloc(numBlocks * sizeof(int));
    for (int i = 0; i < numBlocks; i++)
    {
        lengths[i] = 1;
        displacements[i] = (i % M) * N + (i / M) * size;
    }
    MPI_Type_indexed(numBlocks, lengths, displacements, MPI_INT, &cols);
    MPI_Type_commit(&cols);
    MPI_Type_create_resized(cols, 0, sizeof(int), &cols_resized);
    MPI_Type_commit(&cols_resized);

    MPI_Type_vector(N / size, K, size * K, MPI_INT, &rows);
    MPI_Type_commit(&rows);
    MPI_Type_create_resized(rows, 0, K * sizeof(int), &rows_resized);
    MPI_Type_commit(&rows_resized);

    MPI_Datatype recv_cols, recv_cols_resized;
    MPI_Type_vector(M, 1, (N/size), MPI_INT, &recv_cols);
    MPI_Type_commit(&recv_cols);
    MPI_Type_create_resized(recv_cols, 0, sizeof(int), &recv_cols_resized);
    MPI_Type_commit(&recv_cols_resized);

    MPI_Scatter(&A[0][0], 1, cols_resized, myCols, (N / size), recv_cols_resized, 0, MPI_COMM_WORLD);
    MPI_Scatter(&B[0][0], 1, rows_resized, myRows, (N / size) * K, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

    int max = myCols[0];

    printf("Process %d. My cols:\n", rank);
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N / size; j++)
        {
            printf("%d ", myCols[i * (N / size) + j]);
            if (myCols[i * (N / size) + j] > max)
                max = myCols[i * (N / size) + j];
        }
        printf("\n");
    }
    printf("\n\n");
    fflush(stdout);
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

    printf("Process %d. My rows:\n", rank);
    for (int i = 0; i < N / size; i++)
    {
        for (int j = 0; j < K; j++)
        {
            printf("%d ", myRows[i * K + j]);
        }
        printf("\n");
    }
    printf("\n\n");
    fflush(stdout);
    MPI_Barrier(MPI_COMM_WORLD);

    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < K; j++)
        {
            int res = 0;
            for (int k = 0; k < (N / size); k++)
            {
                res += myCols[i * (N / size) + k] * myRows[k * K + j];
            }
            myRes[i][j] = res;
        }
    }

    struct { int value; int rank; } in, out;
    in.value = max;
    in.rank = rank;

    MPI_Reduce(&in, &out, 1, MPI_2INT, MPI_MAXLOC, 0, MPI_COMM_WORLD);
    MPI_Bcast(&out, 1, MPI_2INT, 0, MPI_COMM_WORLD);
    MPI_Reduce(&myRes[0][0], &C[0][0], (M * K), MPI_INT, MPI_SUM, out.rank, MPI_COMM_WORLD);

    if (rank == out.rank)
    {
        printf("Process %d. Final matrix: \n", rank);
        for (int i = 0; i < M; i++)
        {
            for (int j = 0; j < K; j++)
                printf("%d ", C[i][j]);
            printf("\n");
        }
        printf("\n\n");
    }

    MPI_Finalize();
}

