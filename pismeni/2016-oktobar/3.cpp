#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

#define M 3
#define N 4
#define K 6
#define root 0
#define s 3

int main(int argc, char* argv[])
{
    int A[M][N], B[N][K], C[M][K], rank, myCols[N][s], myResPart[M][s], myMin;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == root)
    {
        printf("Matrix A:\n");
        for (int i = 0; i < M; i++)
        {
            for (int j = 0; j < N; j++)
            {
                A[i][j] = rand() % 5 + 1;
                printf("%d ", A[i][j]);
            }
            printf("\n");
        }

        printf("\nMatrix B:\n");
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < K; j++)
            {
                B[i][j] = rand() % 5 + 1;
                printf("%d ", B[i][j]);
            }
            printf("\n");
        }
        printf("\n");
    }

    MPI_Bcast(&A[0][0], M * N, MPI_INT, root, MPI_COMM_WORLD);
    MPI_Datatype Cols, Cols_resized;
    MPI_Type_vector(N, s, K, MPI_INT, &Cols);
    MPI_Type_commit(&Cols);
    MPI_Type_create_resized(Cols, 0, s * sizeof(int), &Cols_resized);
    MPI_Type_commit(&Cols_resized);

    MPI_Scatter(&B[0][0], 1, Cols_resized, &myCols[0][0], N * s, MPI_INT, root, MPI_COMM_WORLD);

    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < s; j++)
        {
            int res = 0;
            for (int k = 0; k < N; k++)
            {
                res += A[i][k] * myCols[k][j];
            }
            myResPart[i][j] = res;
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    printf("\nProcess %d. My cols part:\n", rank);
    myMin = myCols[0][0];
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < s; j++)
        {
            if (myMin > myCols[i][j])
                myMin = myCols[i][j];
            printf("%d ", myCols[i][j]);
        }
        printf("\n");
    }

    printf("\nProcess %d. My res part:\n", rank);
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < s; j++)
            printf("%d ", myResPart[i][j]);
        printf("\n");
    }
    fflush(stdout);
    MPI_Barrier(MPI_COMM_WORLD);

    struct { int value; int rank; } in, out;

    in.value = myMin;
    in.rank = rank;

    MPI_Reduce(&in, &out, 1, MPI_2INT, MPI_MINLOC, root, MPI_COMM_WORLD);
    MPI_Bcast(&out, 1, MPI_2INT, root, MPI_COMM_WORLD);

    MPI_Datatype Res_cols, Res_cols_resized;
    MPI_Type_vector(M, s, K, MPI_INT, &Res_cols);
    MPI_Type_commit(&Res_cols);
    MPI_Type_create_resized(Res_cols, 0, s * sizeof(int), &Res_cols_resized);
    MPI_Type_commit(&Res_cols_resized);
    MPI_Gather(&myResPart[0][0], M * s, MPI_INT, &C[0][0], 1, Res_cols_resized, out.rank, MPI_COMM_WORLD);


    if (rank == out.rank)
    {
        printf("\nSmallest value: %d\n\n", out.value);

        printf("Final result:\n");
        for (int i = 0; i < M; i++)
        {
            for (int j = 0; j < K; j++)
            {
                printf("%d ", C[i][j]);
            }
            printf("\n");
        }
        printf("\n");
    }

    MPI_Finalize();
}

