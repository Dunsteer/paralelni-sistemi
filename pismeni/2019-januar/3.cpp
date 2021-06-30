#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define N 10

int main()
{
    int m = 1, x[N], y[N], z[N];
    
    for (int i = 0; i < N; i++)
    {
        x[i] = rand() % 5 + 1;
        y[i] = rand() % 6 + 1;
        z[i] = rand() % 7 + 1;
    }
    printf("x: ");
    for (int i = 0; i < N; i++)
        printf("%d ", x[i]);
    printf("\n");
    printf("y: ");
    for (int i = 0; i < N; i++)
        printf("%d ", y[i]);
    printf("\n");
    printf("z: ");
    for (int i = 0; i < N; i++)
        printf("%d ", z[i]);
    printf("\n");


    int a[N];
    #pragma omp parallel for
    for (int i = 0; i < N; i++)
    {
        a[i] = x[i];
    }

    #pragma omp parallel for reduction(*:m)
    for (int i = 0; i < N - 1; i++)
    {
        x[i] = a[i + 1] + y[i] + z[i];
        m = m * x[i];
    }

    printf("Array x: ");
    for (int i = 0; i < N; i++)
        printf("%d ", x[i]);
    printf("\n");
    printf("m: %d\n\n", m);

    m = 1;
    for (int i = 0; i < N; i++)
    {
        x[i] = a[i];
    }

    for (int i = 0; i < N - 1; i++)
    {
        x[i] = x[i + 1] + y[i] + z[i];
        m = m * x[i];
    }

    printf("Array x seq: ");
    for (int i = 0; i < N; i++)
        printf("%d ", x[i]);
    printf("\n");
    printf("m seq: %d\n\n", m);


    
}
