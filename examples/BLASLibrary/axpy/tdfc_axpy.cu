// tdfc-cuda backend autocompiled body file
// tdfc version 1.160
// Wed May 25 15:55:04 2011

#include <stdio.h>


__global__ void tdfc_axpy(float cc_alpha,float* cc_x,float* cc_y_in,float* cc_y_out,int N  )
{
  int idx = blockIdx.x * blockDim.x + threadIdx.x;

  if(idx<N)
  {
          {
            cc_y_out[idx] = (((cc_x[idx]*cc_alpha)+cc_y_in[idx]));
          }
  }
} //tdfc_axpy
