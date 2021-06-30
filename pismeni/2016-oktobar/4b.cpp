#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

void printMatrix(int* mat, int n, int m)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
            printf("%d ", mat[i * m + j]);
        printf("\n");
    }
    printf("\n\n");
}

int main()
{
    const int m = 4, n = 5;
    int j, a[n][m], b[n][m];
    printf("Old a:\n");
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            a[i][j] = rand() % 7 + 1;
            b[i][j] = a[i][j];
            printf("%d ", a[i][j]);
        }
        printf("\n");
    }
    printf("\n\n");

    #pragma omp parallel for private(j)
    for (int i = 0; i < n; i++)
    {
        for (j = 2; j < m; j++)
            a[i][j] = 2 * a[i][j - 2];
    }

    printf("New matrix a:\n");
    printMatrix(&a[0][0], n, m);

    for (j = 2; j < m; j++)
    {
        for (int i = 0; i < n; i++)
            b[i][j] = 2 * a[i][j - 2];
    }

    printf("Test matrix:\n");
    printMatrix(&b[0][0], n, m);
}
