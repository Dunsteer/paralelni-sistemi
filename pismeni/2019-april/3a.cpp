#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char* argv[])
{
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    ///////////////////////////////////////////////////////////////////////////////

    int dim = (int)log2(size);
    int myPartner;
    int* myBuffer = (int*)malloc(size * sizeof(int));
    myBuffer[0] = rank;
    int mySum = rank;
    int* recvBuffer = (int*)malloc((size/2) * sizeof(int));
    int cnt = 1;

    for (int i = 0; i < dim; i++)
    {
        myPartner = rank ^ (int)pow(2, i);
        MPI_Sendrecv(myBuffer, (int)pow(2, i), MPI_INT, myPartner, 0, recvBuffer,
            (int)pow(2, i), MPI_INT, myPartner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for (int j = 0; j < (int)pow(2, i); j++)
        {
            myBuffer[cnt] = recvBuffer[j];
            cnt++;
            if (recvBuffer[j] <= rank)
                mySum += recvBuffer[j];
        }
    }

    printf("Process %d. My Sum: %d. My buffer: ", rank, mySum);
    for (int i = 0; i < size; i++)
        printf("%d ", myBuffer[i]);
    printf("\n\n");
    fflush(stdout);

    free(myBuffer);
    free(recvBuffer);

    ////////////////////////////////////////////////////////////////////////////////////

    //grupne opracije, mozda (???)
    int* myBuffer = (int*)malloc(size * sizeof(int));

    myBuffer[rank] = rank;
    for(int i=0; i<size; i++)
    {
        MPI_Bcast(&myBuffer[i], 1, MPI_INT, i, MPI_COMM_WORLD);
    }

    int mySum = 0;
    for (int i = 0; i <= rank; i++)
        mySum += myBuffer[i];

    printf("Process %d. My Sum: %d. My buffer: ", rank, mySum);
    for (int i = 0; i < size; i++)
        printf("%d ", myBuffer[i]);
    printf("\n\n");
    fflush(stdout);


    MPI_Finalize();
}

