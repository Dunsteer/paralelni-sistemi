#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define N 6

int main()
{
    int m = 4;
    int a[N];
    int j;
    a[0] = m;
    int c = m;
    #pragma omp parallel for reduction(+:m)
    for (j = 0; j < N; j++)
    {
        a[j] = c + ((j - 1) * j) / 2;
        m += j;
        printf("Running\n");
    }

    /*#pragma omp parallel for reduction(+:m)
    for (j = 0; j < N; j++)
    {
        m += j;
    }*/

    printf("M: %d.\n\nA:", m);
    for (int i = 0; i < N; i++)
        printf("%d ", a[i]);
}
