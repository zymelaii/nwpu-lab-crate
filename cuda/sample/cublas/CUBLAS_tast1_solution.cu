#include <cuda_runtime.h>
#include <cublas_v2.h>
#include <time.h>
#include <stdio.h>
#include <curand.h>
#define INDX( row, col, ld ) ( ( (col) * (ld) ) + (row) )
#define SIZE 4096
#define BLKDIM 32
// A single-threaded version of matrix multiply
void host_sgemm( int m, int n, int k, float *a, float *b, float *c ){
  for( int j = 0; j < n; j++ )  {
    for( int i = 0; i < m; i++ )    {
      for( int koff = 0; koff < k; koff++ )      {
        c[INDX(i, j, m)] += a[INDX( i, koff, m )] * b[INDX( koff, j, n )];
      } /* end for i */
    } /* end jb */
  } /* end for j */
} /* end host_sgemm */



  /* Compute r = p * q, for square nxn matrices p, q, r; this version
  does not use shared memory. This kernel does not require that n is
  a multiple of BLKDIM */
__global__ void matmul(const float *p, const float *q, float *r, int n)
{
	const int i = blockIdx.y * blockDim.y + threadIdx.y;
	const int j = blockIdx.x * blockDim.x + threadIdx.x;
	int k;
	float val = 0.0;
	if (i < n && j < n) {
		for (k = 0; k<n; k++) {
			val += p[i*n + k] * q[k*n + j];
		}
		r[i*n + j] = val;
	}
}

/* Same as above, but does not require that n is a multiple of
BLKDIM. To do so, it fills shared buffers so that values outside
the matrices are treated as zeros. */
__global__ void matmulb_generic(const float *p, const float *q, float *r, int n)
{
	__shared__ float local_p[BLKDIM][BLKDIM];
	__shared__ float local_q[BLKDIM][BLKDIM];
	const int bx = blockIdx.x, by = blockIdx.y;
	const int tx = threadIdx.x, ty = threadIdx.y;
	const int i = by * BLKDIM + ty;
	const int j = bx * BLKDIM + tx;
	float v = 0.0; int m, k;
	for (m = 0; m < n; m += BLKDIM) { /* loop over tiles */
		local_p[ty][tx] = local_q[ty][tx] = 0;
		if (i<n && m + tx<n)
			local_p[ty][tx] = p[i*n + (m + tx)];
		if (j<n && m + ty<n)
			local_q[ty][tx] = q[(m + ty)*n + j];

		__syncthreads();

		for (k = 0; k < BLKDIM; k++) { /* loop within tile */
			v += local_p[ty][k] * local_q[k][tx];
		}

		__syncthreads();
	}
	if (i<n && j<n)
		r[i*n + j] = v; /* write result to global memory */
}



int main()
{
    const int size = SIZE;

    fprintf(stdout, "Matrix size is %d\n",size);

    float *a, *b, *c, *cdef;

    size_t numbytes = size * size * sizeof( float );

    cudaEvent_t start, stop; cudaEventCreate(&start);	cudaEventCreate(&stop); float runtime;
	clock_t start_t, finish_t;

    // Allocate all our host-side (CPU) and device-side (GPU) data
    cudaMallocManaged( (void **)&a, numbytes);
    cudaMallocManaged( (void **)&b, numbytes);
    cudaMallocManaged( (void **)&c, numbytes);
    cudaMallocManaged( (void **)&cdef, numbytes);

    if( a == NULL || b == NULL || c == NULL || c == NULL)
    {
      fprintf(stderr,"Error in malloc\n");
      return 911;
    }

    // Clear the result matrices to zero
    memset( c, 0, numbytes );    memset( cdef, 0, numbytes );




	start_t = clock();
    // Initialize the a and b matrices to random data
    for( int i = 0; i < size * size; i++ )
    {
      a[i] = float( rand() ) / ( float(RAND_MAX) + 1.0 );
      b[i] = float( rand() ) / ( float(RAND_MAX) + 1.0 );
    }

	finish_t = clock();
	runtime = (float)(finish_t - start_t) / CLOCKS_PER_SEC * 1000;
	fprintf(stdout, "Initialize random data in CPU: %f ms\n", runtime);


	// First run the CPU verison of dgemm so we can compare the results
	//start_t = clock();
	//printf("Launching CPU sgemm\n");
	//host_sgemm(size, size, size, a, b, cdef);
	//finish_t = clock();
	//runtime = (float)(finish_t - start_t) / CLOCKS_PER_SEC * 1000;
	//fprintf(stdout, "Total time CPU is %f ms\n", runtime);
	//fprintf(stdout, "Performance is %f GFlop/s\n",
	//	2.0 * (double)size * (double)size * (double)size /
	//	((double)runtime / 1000.0) * 1.e-9);


	start_t = clock();
	// Generate size * size random numbers
	printf("Create random numbers\n");
	// Create pseudo-random number generator
	curandGenerator_t gen;
	curandCreateGeneratorHost(&gen, CURAND_RNG_PSEUDO_DEFAULT);
	curandGenerateNormal(gen, a, size*size, 0.0, float(RAND_MAX));
	curandGenerateNormal(gen, b, size*size, 0.0, float(RAND_MAX));
	finish_t = clock();
	runtime = (float)(finish_t - start_t) / CLOCKS_PER_SEC * 1000;
	fprintf(stdout, "Initialize via cuRAND on CPU: %f ms\n", runtime);


  start_t = clock();
	// Generate size * size random numbers
	printf("Create random numbers\n");
	// Create pseudo-random number generator
	curandGenerator_t gen1;
	curandCreateGenerator(&gen1, CURAND_RNG_PSEUDO_DEFAULT);
	curandGenerateNormal(gen1, a, size*size, 0.0, float(RAND_MAX));
	curandGenerateNormal(gen1, b, size*size, 0.0, float(RAND_MAX));
	finish_t = clock();
	runtime = (float)(finish_t - start_t) / CLOCKS_PER_SEC * 1000;
  fprintf(stdout, "Initialize via cuRAND on GPU: %f ms\n", runtime);






	/**
	** Matrix-matrix multiply WITHOUT shared memory
	**/
	dim3 block(BLKDIM, BLKDIM);
	dim3 grid((SIZE + BLKDIM - 1) / BLKDIM, (SIZE + BLKDIM - 1) / BLKDIM);
	printf("No shared memory:\t");
	cudaEventCreate(&start);	cudaEventCreate(&stop);	cudaEventRecord(start, 0);
	matmul << <grid, block >> >(a, b, c, SIZE);	cudaDeviceSynchronize();
	cudaEventRecord(stop, 0);	cudaEventSynchronize(stop);	cudaEventElapsedTime(&runtime, start, stop);
	printf("GPU Elapsed time:%.6f ms.\n", runtime);
	fprintf(stdout, "Performance is %f GFlop/s\n",
		2.0 * (double)size * (double)size * (double)size /
		((double)runtime / 1000.0) * 1.e-9);

	/**
	** Matrix-matrix multiply WITH shared memory
	**/
	printf("Shared memory:\t");
	cudaEventCreate(&start);	cudaEventCreate(&stop);	cudaEventRecord(start, 0);
	matmulb_generic << <grid, block >> >(a, b, c, SIZE);    cudaDeviceSynchronize();
	cudaEventRecord(stop, 0);	cudaEventSynchronize(stop);	cudaEventElapsedTime(&runtime, start, stop);
	printf("GPU Elapsed time:%.6f ms.\n", runtime);
	fprintf(stdout, "Performance is %f GFlop/s\n",
		2.0 * (double)size * (double)size * (double)size /
		((double)runtime / 1000.0) * 1.e-9);

    // Now run the GPU version of sgemm using the cuBLAS library
    cublasHandle_t handle;   cublasStatus_t stat = cublasCreate( &handle );

    // Set these constants so we get a simple matrix multiply with cublasDgemm
    float alpha = 1.0;    float beta  = 0.0;

    // Launch cublasSgemm on the GPU
    printf("Launching GPU sgemm\n");    cudaEventRecord(start, 0);
    cublasSgemm( handle, CUBLAS_OP_N, CUBLAS_OP_N,
                 size, size, size,
                 &alpha,
                 a, size,
                 b, size,
                 &beta,
                 c, size );
    cudaEventRecord(stop, 0);	cudaEventSynchronize(stop);
    cudaEventElapsedTime(&runtime, start, stop);


    fprintf(stdout, "Total time GPU CUBLAS is %f ms\n", runtime );
    fprintf(stdout, "Performance is %f GFlop/s\n",
      2.0 * (double) size * (double) size * (double) size /
      ( (double) runtime / 1000.0 ) * 1.e-9 );

    cublasDestroy( handle );
	curandDestroyGenerator(gen);curandDestroyGenerator(gen1);

    // Do some error checking to verify our GPU & CPU verisons are within
    // an acceptable error bound
    float temp = 0.0;
    for( int i = 0; i < size * size; i++ )
    {
        temp += ( c[i] - cdef[i] ) * ( c[i] - cdef[i] );
    } /* end for */

    printf("error is %f\n",temp);
    if( temp > 10 ) printf("Error value is suspiciously high!\n");

    cudaFree( a );
    cudaFree( b );
    cudaFree( c );
    cudaFree( cdef );

    return 0;
}
