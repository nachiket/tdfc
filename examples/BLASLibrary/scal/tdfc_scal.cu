// tdfc-cuda backend autocompiled body file
// tdfc version 1.160
// Thu May 26 17:01:38 2011

#include <stdio.h>


__global__ void tdfc_scal(float cc_alpha,float* cc_x,float* cc_x_out,int N  )
{
  int idx = blockIdx.x * blockDim.x + threadIdx.x;

  if(idx<N)
  {
          {
            cc_x_out[idx] = ((cc_x[idx]*cc_alpha));
          }
  }
} //tdfc_scal
