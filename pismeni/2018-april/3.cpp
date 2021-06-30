#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

#define N 6
#define M 3

int main(int argc, char* argv[])
{
    int rank, size, A[M][N], b[N], c[M], myResPart[M];
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int* myCols = (int*)malloc((N / size) * M * sizeof(int)),
        * myVectorPart = (int*)malloc((N / size) * sizeof(int));

    if (rank == 0)
    {
        printf("Matrix A:\n");
        for (int i = 0; i < M; i++)
        {
            for (int j = 0; j < N; j++)
            {
                A[i][j] = rand() % 6 + 1;
                printf("%d ", A[i][j]);
            }
            printf("\n");
        }
        printf("\n\n");

        printf("Vector B: ");
        for (int i = 0; i < N; i++)
        {
            b[i] = rand() % 6 + 1;
            printf("%d ", b[i]);
        }
        printf("\n\n");
    }

    MPI_Datatype cols, cols_resized, cols_recv, cols_recv_resized, send;
    MPI_Type_vector(M, 1, N, MPI_INT, &cols);
    MPI_Type_commit(&cols);
    MPI_Type_create_resized(cols, 0, size * sizeof(int), &cols_resized);
    MPI_Type_commit(&cols_resized);

    MPI_Type_vector(M, 1, N / size, MPI_INT, &cols_recv);
    MPI_Type_commit(&cols_recv);
    MPI_Type_create_resized(cols_recv, 0, sizeof(int), &cols_recv_resized);
    MPI_Type_commit(&cols_recv_resized);

    MPI_Type_vector(N / size, 1, size, MPI_INT, &send);
    MPI_Type_commit(&send);

    if (rank == 0)
    {
        for (int i = 0; i < M; i++)
        {
            for (int j = 0; j < N / size; j++)
            {
                myCols[i * (N / size) + j] = A[i][j * size];
            }
        }

        for (int i = 0; i < N / size; i++)
        {
            myVectorPart[i] = b[i * size];
        }

        for (int i = 1; i < size; i++)
        {
            //probaj ono drugo
            MPI_Send(&A[0][i], N / size, cols_resized, i, 0, MPI_COMM_WORLD);
            MPI_Send(&b[i], 1, send, i, 0, MPI_COMM_WORLD);
        }
    }
    else
    {
        MPI_Recv(myCols, N / size, cols_recv_resized, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(myVectorPart, N / size, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    printf("Process %d. My matrix part:\n", rank);
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N / size; j++)
            printf("%d ", myCols[i * (N / size) + j]);
        printf("\n");
    }
    printf("\n");
    printf("Process %d. My vector part:\n", rank);
    for (int i = 0; i < N / size; i++)
        printf("%d ", myVectorPart[i]);
    printf("\n\n");
    fflush(stdout);
    MPI_Barrier(MPI_COMM_WORLD);

    for (int i = 0; i < M; i++)
    {
        int res = 0;
        for (int j = 0; j < N / size; j++)
        {
            res += myCols[i * (N / size) + j] * myVectorPart[j];
        }
        myResPart[i] = res;
    }

    MPI_Reduce(myResPart, c, M, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        printf("Final vector: ");
        for (int i = 0; i < M; i++)
            printf("%d ", c[i]);
        printf("\n");
    }

    MPI_Finalize();
    free(myVectorPart);
    free(myCols);
}

