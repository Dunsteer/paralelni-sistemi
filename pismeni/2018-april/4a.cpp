#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char* argv[])
{
    int rank, mySum, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    mySum = rank;
    int i = 0;
    int dim = log2(size);
    int* myBuffer = (int*)malloc(size * sizeof(int));
    myBuffer[0] = rank;

    int* myRecvBuffer = (int*)malloc(pow(2, size - 1) * sizeof(int));

    int sendSize = 1;
    int myPartner = rank ^ 1;

    while (i < dim)
    {
        MPI_Sendrecv(myBuffer, pow(2, i), MPI_INT, myPartner, 0, myRecvBuffer,
            pow(2, i), MPI_INT, myPartner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        /*if (rank == 2)
        {
            printf("Got: ");
            for (int j = 0; j < pow(2, i); j++)
                printf("%d ", myRecvBuffer[j]);
            printf("\n");
        }*/
        
        for (int j = 0; j < pow(2, i); j++)
        {
            if(myRecvBuffer[j] <= rank)
            {
                mySum += myRecvBuffer[j];
            }
        }

        for (int j = 0; j < pow(2, i); j++)
        {
            myBuffer[(int)pow(2, i) + j] = myRecvBuffer[j];
        }

        i++;
        myPartner = rank ^ (int)pow(2, i);
    }

    printf("Process %d. My sum: %d. My buffer:\n", rank, mySum);
    for (int i = 0; i < size; i++)
    {
        printf("%d ", myBuffer[i]);
    }
    printf("\n\n");

    MPI_Finalize();
    free(myRecvBuffer);
    free(myBuffer);
}

