// tdfc-cuda backend autocompiled body file
// tdfc version 1.160
// Thu May 26 16:38:16 2011

#include <stdio.h>


__global__ void tdfc_rot(float cc_c,float cc_s,float* cc_x,float* cc_y,float* cc_x_out,float* cc_y_out,int N  )
{
  int idx = blockIdx.x * blockDim.x + threadIdx.x;

  if(idx<N)
  {
          {
            cc_x_out[idx] = (((cc_x[idx]*cc_c)+(cc_y[idx]*cc_s)));
            cc_y_out[idx] = (((cc_y[idx]*cc_c)-(cc_x[idx]*cc_s)));
          }
  }
} //tdfc_rot
