#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define N 15

int main()
{
    const int limit = (N * (N + 1)) / 2;
    int i1 = 0, i2 = 0, k;
    int a[limit], b[N-1];
    #pragma omp parallel for reduction(+:i2)
    for (k = 1; k < N; k++)
    {
        b[k - 1] = k * 2;
        printf("cao\n");
        i2 += k;
        a[(k * (k + 1)) / 2] = k + 2;
    }

    i1 = N - 1;


    printf("i1: %d\n", i1);
    printf("i2: %d\n", i2);
    printf("B: ");
    for (int i = 0; i < N - 1; i++)
        printf("%d ", b[i]);
    printf("\nA: ");
    for (int i = 0; i < limit; i++)
        printf("%d ", a[i]);

    printf("\n\n-----Sequential-----\n");
    i1 = 0; 
    i2 = 0;
    for (k = 1; k < N; k++)
    {
        b[i1++] = k * 2;
        i2 += k;
        a[i2] = k + 2;
    }
        
    printf("i1: %d\n", i1);
    printf("i2: %d\n", i2);
    printf("B: ");
    for (int i = 0; i < N - 1; i++)
        printf("%d ", b[i]);
    printf("\nA: ");
    for (int i = 0; i < limit; i++)
        printf("%d ", a[i]);
}
