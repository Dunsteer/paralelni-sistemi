#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define N 5

int main()
{
    int A[N][N];
    double B[N][N];

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


    int mySum = 0;
    double average = 0;
    #pragma omp parallel firstprivate(mySum) reduction(+:average)
    {
        #pragma omp for
        for (int i = 0; i < N * N; i++)
        {
            mySum += A[i / N][i % N];
        }
        average = (double)mySum / (N * N);
    }

    printf("Average: %lf\n\n", average);


    #pragma omp parallel for
    for (int i = 0; i < N * N; i++)
    {
        B[i / N][i % N] = A[i / N][i % N] + average;
    }

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
            printf("%lf ", B[i][j]);
        printf("\n");
    }
    printf("\n\n");

    int sum = 0;
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            sum += A[i][j];
        }
    }

    printf("Average seq: %lf\n\n", (double)sum / (N * N));
}
