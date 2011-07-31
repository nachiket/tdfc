// tdfc-cuda backend autocompiled body file
// tdfc version 1.160
// Thu May 26 15:56:56 2011

#include <stdio.h>


__global__ void tdfc_copy(float* cc_x,float* cc_y,int N  )
{
  int idx = blockIdx.x * blockDim.x + threadIdx.x;

  if(idx<N)
  {
          {
            cc_y[idx] = (cc_x[idx]);
          }
  }
} //tdfc_copy
