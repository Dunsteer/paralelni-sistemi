#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

#define N 10

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
                A[i][j] = rand() % 7 + 1;
                printf("%d ", A[i][j]);
            }
            printf("\n");
        }
        printf("\n\n");
    }

    MPI_Datatype send, send_resized, recv;
    MPI_Type_vector(N / 2, N / 2, N, MPI_INT, &send);
    MPI_Type_commit(&send);
    MPI_Type_create_resized(send, 0, 0, &send_resized);
    MPI_Type_commit(&send_resized);

    const int numOfEl = (N * N) / 2;
    int lengths[N], displacements[N];
    int cnt = 0, num = 1, iter = 1;
    while (cnt < numOfEl)
    {
        if ((cnt + num) > numOfEl)
            num = numOfEl - cnt;
        lengths[iter - 1] = num;
        displacements[iter - 1] = iter * N - iter;
        cnt += num;
        num++;
        iter++;
    }
    
    MPI_Type_indexed(N, lengths, displacements, MPI_INT, &recv);
    MPI_Type_commit(&recv);

    if (rank == 0)
    {
        MPI_Send(&A[0][N/2], 2, send_resized, 1, 0, MPI_COMM_WORLD);
        MPI_Send(&A[N / 2][0], 2, send_resized, 2, 0, MPI_COMM_WORLD);
        MPI_Send(&A[N / 2][N / 2], 2, send_resized, 3, 0, MPI_COMM_WORLD);
    }

    else if (rank == 1 || rank == 2 || rank == 3)
    {
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
                C[i][j] = 0;
        }

        MPI_Recv(&C[0][0], 1, recv, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("Process %d. My matrix: \n", rank);
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                printf("%d ", C[i][j]);
            }
            printf("\n");
        }
        printf("\n\n");
    }

    MPI_Finalize();

}

