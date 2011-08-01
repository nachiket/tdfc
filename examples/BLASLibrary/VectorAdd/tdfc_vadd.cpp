// tdfc-cuda autocompiled wrapper file
// tdfc version 1.160
// Fri May 27 17:47:08 2011

#include <stdio.h>
#include <cuda.h>
#include "tdfc_vadd.cuh"

int main(void)
{
  const double cc_a_d = (double)1;
  double *cc_x_h, *cc_x_d;
  double *cc_y_h, *cc_y_d;
  double *cc_z_h, *cc_z_d;
  const int N = 1024; // Number of elements in array

  // cc_x
  size_t cc_x_size = N * sizeof(double);
  cc_x_h = (double *)malloc(cc_x_size);
  cudaMalloc((void **) &cc_x_d, cc_x_size);

  // cc_y
  size_t cc_y_size = N * sizeof(double);
  cc_y_h = (double *)malloc(cc_y_size);
  cudaMalloc((void **) &cc_y_d, cc_y_size);

  // cc_z
  size_t cc_z_size = N * sizeof(double);
  cc_z_h = (double *)malloc(cc_z_size);
  cudaMalloc((void **) &cc_z_d, cc_z_size);

  // Initialize input contents and copy to Device memory
  for(int i=0; i<N; i++) {
    cc_x_h[i] = (double)i;
    cc_y_h[i] = (double)i;
  }

  //Performance Calculation
#ifdef PERF
  cudaEvent_t start, stop;
  float time;
  cudaEventCreate(&start);
  cudaEventCreate(&stop);
  cudaEventRecord(start, 0);
#endif

  cudaMemcpy(cc_x_d, cc_x_h, cc_x_size, cudaMemcpyHostToDevice);
  cudaMemcpy(cc_y_d, cc_y_h, cc_y_size, cudaMemcpyHostToDevice);

  // Do calculation on Device
  int block_size = 8;
  int n_blocks = N/block_size + (N%block_size == 0 ? 0:1);

  tdfc_vadd <<< n_blocks, block_size >>> (cc_a_d, cc_x_d, cc_y_d, cc_z_d, N);

  // Retrieve results
  cudaMemcpy(cc_z_h, cc_z_d, cc_z_size, cudaMemcpyDeviceToHost);

#ifdef PERF
  cudaEventRecord(stop, 0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&time, start, stop);
  cudaEventDestroy(start);
  cudaEventDestroy(stop);
  printf("GPU time for tdfc_vadd is %f ms\n", time);
#endif

  // Print results (typecasted to prevent printf errors)
  printf("tdfc_vadd:\n");
  for (int i=0; i<N; i++) printf("cc_z_h[%d] = %f\n", i, (float)cc_z_h[i]);

  // Cleanup
  free(cc_x_h);   cudaFree(cc_x_d);
  free(cc_y_h);   cudaFree(cc_y_d);
  free(cc_z_h);   cudaFree(cc_z_d);

} // main
