% % cu
#include <iostream>
#include <cuda.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define N 9
#define BLOCKSIZE 4

        __global__ void
        kernel(float *a, float *b)
{
  __shared__ float arrayPart[BLOCKSIZE + 2];
  int mySharedId = blockDim.x * blockIdx.x + threadIdx.x;
  if (mySharedId < (N + 2))
  {
    arrayPart[threadIdx.x] = a[mySharedId];
    if (threadIdx.x >= BLOCKSIZE - 2 && mySharedId + 2 < (N + 2))
      arrayPart[threadIdx.x + 2] = a[mySharedId + 2];
  }

  __syncthreads();

  if (mySharedId < N)
    b[mySharedId] = (3 * arrayPart[threadIdx.x] + 10 * arrayPart[threadIdx.x + 1] + 7 * arrayPart[threadIdx.x + 2]) / 20.f;
}

int main(void)
{
  float A[N + 2], B[N];

  float *device_a, *device_b;
  cudaMalloc((void **)&device_a, (N + 2) * sizeof(float));
  cudaMalloc((void **)&device_b, N * sizeof(float));
  for (int i = 0; i < N + 2; i++)
  {
    A[i] = i + 2;
    printf("%f ", A[i]);
  }

  cudaMemcpy(device_a, A, (N + 2) * sizeof(int), cudaMemcpyHostToDevice);

  int blockSize = BLOCKSIZE;
  int gridSize = (int)ceil(((float)N) / blockSize);
  printf("\ngridSize: %d\n", gridSize);

  kernel<<<gridSize, blockSize>>>(device_a, device_b);
  cudaMemcpy(B, device_b, N * sizeof(float), cudaMemcpyDeviceToHost);

  printf("A: ");
  for (int i = 0; i < N + 2; i++)
    printf("%f ", A[i]);
  printf("\n\nB: ");
  for (int i = 0; i < N; i++)
    printf("%f ", B[i]);
  printf("\n\n");
}