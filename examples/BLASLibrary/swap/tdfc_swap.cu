// tdfc-cuda backend autocompiled body file
// tdfc version 1.160
// Thu May 26 17:25:03 2011

#include <stdio.h>


__global__ void tdfc_swap(float* cc_x,float* cc_y,float* cc_x_out,float* cc_y_out,int N  )
{
  int idx = blockIdx.x * blockDim.x + threadIdx.x;

  if(idx<N)
  {
          {
            float cc_temp=cc_x[idx];
            cc_x_out[idx] = (cc_y[idx]);
            cc_y_out[idx] = (cc_temp);
          }
  }
} //tdfc_swap
