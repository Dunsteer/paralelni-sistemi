#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

#define N 6

int main(int argc, char* argv[])
{
    int rank, A[N][N], C[N][N], Test[N/2][N];
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
    {
        printf("Matrix A:\n");
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                A[i][j] = rand() % 10 + 1;
                printf("%d ", A[i][j]);
            }
            printf("\n");
        }
        printf("\n");
    }

    MPI_Datatype Rows, Recv_type, Recv_type_resized;
    MPI_Type_vector(N / 2, N, 2 * N, MPI_INT, &Rows);
    MPI_Type_commit(&Rows);

    int block_lengths[N], displacemenets[N], cnt = 0;
    for (int i = 0; i < N; i++)
    {
        block_lengths[i] = 1;
        if ((i % 2) == 0)
            displacemenets[i] = cnt;
        else
        {
            displacemenets[i] = N + cnt;
            cnt++;
        }
    }

    fflush(stdout);
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Type_indexed(N, block_lengths, displacemenets, MPI_INT, &Recv_type);
    MPI_Type_commit(&Recv_type);
    MPI_Type_create_resized(Recv_type, 0, 2 * N * sizeof(int), &Recv_type_resized);
    MPI_Type_commit(&Recv_type_resized);

    if (rank == 0)
    {
        MPI_Send(&A[0][0], 1, Rows, 1, 0, MPI_COMM_WORLD);
    }

    else if (rank == 1)
    {
        for (int i = 0; i < N; i++)
        {
            for (int j = N / 2; j < N; j++)
                C[i][j] = 0;
        }

        MPI_Recv(&C[0][0], N / 2, Recv_type_resized, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Matrix C:\n");
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

