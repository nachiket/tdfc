// tdfc-cuda autocompiled wrapper file
// tdfc version 1.160
// Wed May 25 15:55:04 2011

#include <stdio.h>
#include <cuda.h>
#include "tdfc_axpy.cuh"

int main(void)
{
  const float cc_alpha_d = (float)1;
  float *cc_x_h, *cc_x_d;
  float *cc_y_in_h, *cc_y_in_d;
  float *cc_y_out_h, *cc_y_out_d;
  const int N = 8; // Number of elements in array

  // cc_x
  size_t cc_x_size = N * sizeof(float);
  cc_x_h = (float *)malloc(cc_x_size);
  cudaMalloc((void **) &cc_x_d, cc_x_size);

  // cc_y_in
  size_t cc_y_in_size = N * sizeof(float);
  cc_y_in_h = (float *)malloc(cc_y_in_size);
  cudaMalloc((void **) &cc_y_in_d, cc_y_in_size);

  // cc_y_out
  size_t cc_y_out_size = N * sizeof(float);
  cc_y_out_h = (float *)malloc(cc_y_out_size);
  cudaMalloc((void **) &cc_y_out_d, cc_y_out_size);

  // Initialize input contents and copy to Device memory
  for(int i=0; i<N; i++) {
    cc_x_h[i] = (float)i;
    cc_y_in_h[i] = (float)i;
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
  cudaMemcpy(cc_y_in_d, cc_y_in_h, cc_y_in_size, cudaMemcpyHostToDevice);

  // Do calculation on Device
  int block_size = 8;
  int n_blocks = N/block_size + (N%block_size == 0 ? 0:1);

  tdfc_axpy <<< n_blocks, block_size >>> (cc_alpha_d, cc_x_d, cc_y_in_d, cc_y_out_d, N);

  // Retrieve results
  cudaMemcpy(cc_y_out_h, cc_y_out_d, cc_y_out_size, cudaMemcpyDeviceToHost);

#ifdef PERF
  cudaEventRecord(stop, 0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&time, start, stop);
  cudaEventDestroy(start);
  cudaEventDestroy(stop);
  printf("GPU time for tdfc_axpy is %f ms\n", time);
#endif

  // Print results (typecasted to prevent printf errors)
  printf("tdfc_axpy:\n");
  for (int i=0; i<N; i++) printf("cc_y_out_h[%d] = %f\n", i, (float)cc_y_out_h[i]);

  // Cleanup
  free(cc_x_h);   cudaFree(cc_x_d);
  free(cc_y_in_h);   cudaFree(cc_y_in_d);
  free(cc_y_out_h);   cudaFree(cc_y_out_d);

} // main
