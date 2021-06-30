#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

#define N 6
#define K 2


int main(int argc, char* argv[])
{
    int rank, A[N][N], B[N][N], myRows[K][N], myCols[N][K], myRes[K][K], C[N][N];
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
    {
        printf("Matrix A:\n");
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                A[i][j] = rand() % 7 + 1;
                printf("%d ", A[i][j]);
            }
            printf("\n");
        }
        printf("\n\n");

        printf("Matrix B:\n");
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                B[i][j] = rand() % 7 + 1;
                printf("%d ", B[i][j]);
            }
            printf("\n");
        }
        printf("\n\n");
    }


    const int newSize = (N / K) * (N / K);

    MPI_Datatype rows, rows_resized, cols, cols_recv;
    MPI_Type_vector(K, N, N, MPI_INT, &rows);
    MPI_Type_commit(&rows);
    MPI_Type_create_resized(rows, 0, N * K * sizeof(int), &rows_resized);
    MPI_Type_commit(&rows_resized);

    MPI_Type_vector(N, K, N, MPI_INT, &cols);
    MPI_Type_commit(&cols);

    if (rank == 0)
    {
        for (int i = 0; i < N / K; i++)
        {
            for (int j = 0; j < N / K; j++)
            {
                MPI_Send(&B[0][j * K], 1, cols, i * (N / K) + j + 1, 1, MPI_COMM_WORLD);
            }
        }
    }
    else
    {
        MPI_Recv(myCols, N * K, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    for (int i = 0; i < N / K; i++)
    {
        MPI_Group groupWorld, groupRow;
        MPI_Comm commRow;
        MPI_Comm_group(MPI_COMM_WORLD, &groupWorld);
        int incl[N / K + 1];
        incl[0] = 0;
        for (int j = 1; j <= N / K; j++)
        {
            incl[j] = i * (N / K) + j;
        }
        MPI_Group_incl(groupWorld, N / K + 1, incl, &groupRow);
        MPI_Comm_create(MPI_COMM_WORLD, groupRow, &commRow);
        if (rank == 0 || (rank > (i * (N / K)) && rank <= (i + 1) * (N/K)))
        {
            MPI_Bcast(&A[i * K][0], 1, rows, 0, commRow);
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }

    if (rank != 0)
    {
        for (int i = 0; i < K; i++)
        {
            for (int j = 0; j < N; j++)
            {
                myRows[i][j] = A[((rank - 1) / (N / K)) * K + i][j];
            }
        }
    }
    if (rank == 4)
    {
        printf("\n\n\n");
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
                printf("%d ", A[i][j]);
            printf("\n");
        }
        printf("\n\n\n");
    }

    printf("Process %d. My rows:\n", rank);
    for (int i = 0; i < K; i++)
    {
        for (int j = 0; j < N; j++)
            printf("%d ", myRows[i][j]);
        printf("\n");
    }
    printf("\nProcess %d. My cols:\n", rank);
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < K; j++)
            printf("%d ", myCols[i][j]);
        printf("\n");
    }
    printf("\n\n");
    fflush(stdout);
    MPI_Barrier(MPI_COMM_WORLD);

    if(rank != 0)
    {
        for (int i = 0; i < K; i++)
        {
            for (int j = 0; j < K; j++)
            {
                int res = 0;
                for (int k = 0; k < N; k++)
                {
                    res += myRows[i][k] * myCols[k][j];
                }
                myRes[i][j] = res;
            }
        }
    }
    
    MPI_Datatype recv, recv_resized;
    MPI_Type_vector(K, K, N, MPI_INT, &recv);
    MPI_Type_commit(&recv);
    MPI_Type_create_resized(recv, 0, K * sizeof(int), &recv_resized);
    MPI_Type_commit(&recv_resized);

    if (rank != 0)
    {
        MPI_Send(&myRes[0][0], K * K, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    else
    {
        for (int i = 0; i < (N / K); i++)
        {
            for (int j = 0; j < (N / K); j++)
            {
                MPI_Recv(&C[i * K][j * K], 1, recv, i * (N / K) + j + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }

        printf("Matrix C:\n");
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
                printf("%d ", C[i][j]);
            printf("\n");
        }
        printf("\n\n");
    }

    MPI_Finalize();
}

