#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

#define N 8

int main(int argc, char* argv[])
{
    int rank, A[N][N], C[N][N];
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
    {
        printf("Matrix A:\n");
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                A[i][j] = rand() % 9 + 1;
                printf("%d ", A[i][j]);
            }
            printf("\n");
        }
    }

    MPI_Datatype Send, Send_resized, Recv;
    MPI_Type_vector((N * N) / 2, 1, 2, MPI_INT, &Send);
    MPI_Type_commit(&Send);
    /*MPI_Type_create_resized(Send, 0, 2 * N * sizeof(int), &Send_resized);
    MPI_Type_commit(&Send_resized);*/

    int displacements[(N * N) / 2], lengths[(N * N) / 2];
    for (int i = 0; i < (N * N) / 2; i++)
    {
        lengths[i] = 1;
    }

    int cnt = 0, offset = 1, limit = N;
    while (cnt < ((N * N) / 2))
    {
        for (int i = 1; i <= limit; i++)
        {
            displacements[cnt] = i * N - offset;
            cnt++;
            if (cnt >= ((N * N) / 2))
                break;
        }
        limit--;
        offset++;
    }

    MPI_Type_indexed((N * N) / 2, lengths, displacements, MPI_INT, &Recv);
    MPI_Type_commit(&Recv);

    if (rank == 0)
    {
        MPI_Send(&A[0][0], 1, Send, 1, 0, MPI_COMM_WORLD);
    }

    else if (rank == 1)
    {
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
                C[i][j] = 0;
        }

        MPI_Recv(&C[0][0], 1, Recv, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("\n\nMatrix C:\n");
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

