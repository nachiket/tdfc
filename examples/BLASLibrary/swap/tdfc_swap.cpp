// tdfc-cuda autocompiled wrapper file
// tdfc version 1.160
// Thu May 26 17:25:03 2011

#include <stdio.h>
#include <cuda.h>
#include "tdfc_swap.cuh"

int main(void)
{
  float *cc_x_h, *cc_x_d;
  float *cc_y_h, *cc_y_d;
  float *cc_x_out_h, *cc_x_out_d;
  float *cc_y_out_h, *cc_y_out_d;
  const int N = 1024; // Number of elements in array

  // cc_x
  size_t cc_x_size = N * sizeof(float);
  cc_x_h = (float *)malloc(cc_x_size);
  cudaMalloc((void **) &cc_x_d, cc_x_size);

  // cc_y
  size_t cc_y_size = N * sizeof(float);
  cc_y_h = (float *)malloc(cc_y_size);
  cudaMalloc((void **) &cc_y_d, cc_y_size);

  // cc_x_out
  size_t cc_x_out_size = N * sizeof(float);
  cc_x_out_h = (float *)malloc(cc_x_out_size);
  cudaMalloc((void **) &cc_x_out_d, cc_x_out_size);

  // cc_y_out
  size_t cc_y_out_size = N * sizeof(float);
  cc_y_out_h = (float *)malloc(cc_y_out_size);
  cudaMalloc((void **) &cc_y_out_d, cc_y_out_size);

  // Initialize input contents and copy to Device memory
  for(int i=0; i<N; i++) {
    cc_x_h[i] = (float)i;
    cc_y_h[i] = (float)i;
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

  tdfc_swap <<< n_blocks, block_size >>> (cc_x_d, cc_y_d, cc_x_out_d, cc_y_out_d, N);

  // Retrieve results
  cudaMemcpy(cc_x_out_h, cc_x_out_d, cc_x_out_size, cudaMemcpyDeviceToHost);
  cudaMemcpy(cc_y_out_h, cc_y_out_d, cc_y_out_size, cudaMemcpyDeviceToHost);

#ifdef PERF
  cudaEventRecord(stop, 0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&time, start, stop);
  cudaEventDestroy(start);
  cudaEventDestroy(stop);
  printf("GPU time for tdfc_swap is %f ms\n", time);
#endif

  // Print results (typecasted to prevent printf errors)
  printf("tdfc_swap:\n");
  for (int i=0; i<N; i++) printf("cc_x_out_h[%d] = %f\n", i, (float)cc_x_out_h[i]);
  for (int i=0; i<N; i++) printf("cc_y_out_h[%d] = %f\n", i, (float)cc_y_out_h[i]);

  // Cleanup
  free(cc_x_h);   cudaFree(cc_x_d);
  free(cc_y_h);   cudaFree(cc_y_d);
  free(cc_x_out_h);   cudaFree(cc_x_out_d);
  free(cc_y_out_h);   cudaFree(cc_y_out_d);

} // main
