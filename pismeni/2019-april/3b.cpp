#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define N 20

int main()
{
    int a[N];
    printf("Array a: ");
    for (int i = 0; i < N; i++)
    {
        a[i] = rand() % 15 + 1;
        printf("%d ", a[i]);
    }
    printf("\n\n");

    int minimum = a[0];
    int finalMin;
    #pragma omp parallel firstprivate(minimum)
    {
        #pragma omp single
        {
            finalMin = a[0];
        }

        #pragma omp for
        for (int i = 1; i < N; i++)
        {
            if (a[i] < minimum)
                minimum = a[i];
        }

        #pragma omp critical
        {
            if (minimum < finalMin)
                finalMin = minimum;
        }
    }

    printf("Final min no reduction: %d.\n", finalMin);


    /////////////////////////////////////////////////////////////////////////////////////


    minimum = a[0];
    #pragma omp parallel for reduction(min:minimum)
    for (int i = 1; i < N; i++)
    {
        if (a[i] < minimum)
            minimum = a[i];
    }

    printf("Final min reduction: %d.\n", minimum);
}
