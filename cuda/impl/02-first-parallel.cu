#include <stdio.h>

/*
 * Refactor firstParallel so that it can run on the GPU.
 */
__global__
void firstParallel()
{
  // printf("This should be running in parallel.\n");
  printf("Yes, i'm running in parallel <%d, %d>\n", blockIdx.x, threadIdx.x);
}

int main()
{
  /*
   * Refactor this call to firstParallel to execute in parallel
   * on the GPU.
   */

  firstParallel<<<3, 3>>>();
  cudaDeviceSynchronize();

  /*
   * Some code is needed below so that the CPU will wait
   * for the GPU kernels to complete before proceeding.
   */

}
