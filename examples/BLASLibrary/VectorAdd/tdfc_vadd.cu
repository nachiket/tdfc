// tdfc-cuda backend autocompiled body file
// tdfc version 1.160
// Fri May 27 17:47:08 2011

#include <stdio.h>


__global__ void tdfc_vadd(double cc_a,double* cc_x,double* cc_y,double* cc_z,int N  )
{
  int idx = blockIdx.x * blockDim.x + threadIdx.x;

  if(idx<N)
  {
          {
            cc_z[idx] = (((cc_a*cc_x[idx])+cc_y[idx]));
          }
  }
} //tdfc_vadd
