// Verilog top module for tpose_d1_ScOrEtMp47
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:43 2009

`include "tpose_d1_ScOrEtMp47_fsm.v"
`include "tpose_d1_ScOrEtMp47_dp.v"

module tpose_d1_ScOrEtMp47 (clock, reset, a0_d, a0_e, a0_v, a0_b, a1_d, a1_e, a1_v, a1_b, a2_d, a2_e, a2_v, a2_b, a3_d, a3_e, a3_v, a3_b, a4_d, a4_e, a4_v, a4_b, a5_d, a5_e, a5_v, a5_b, a6_d, a6_e, a6_v, a6_b, a7_d, a7_e, a7_v, a7_b, b0_d, b0_e, b0_v, b0_b, b1_d, b1_e, b1_v, b1_b, b2_d, b2_e, b2_v, b2_b, b3_d, b3_e, b3_v, b3_b, b4_d, b4_e, b4_v, b4_b, b5_d, b5_e, b5_v, b5_b, b6_d, b6_e, b6_v, b6_b, b7_d, b7_e, b7_v, b7_b);

  input  clock;
  input  reset;

  input  [15:0] a0_d;
  input  a0_e;
  input  a0_v;
  output a0_b;
  input  [15:0] a1_d;
  input  a1_e;
  input  a1_v;
  output a1_b;
  input  [15:0] a2_d;
  input  a2_e;
  input  a2_v;
  output a2_b;
  input  [15:0] a3_d;
  input  a3_e;
  input  a3_v;
  output a3_b;
  input  [15:0] a4_d;
  input  a4_e;
  input  a4_v;
  output a4_b;
  input  [15:0] a5_d;
  input  a5_e;
  input  a5_v;
  output a5_b;
  input  [15:0] a6_d;
  input  a6_e;
  input  a6_v;
  output a6_b;
  input  [15:0] a7_d;
  input  a7_e;
  input  a7_v;
  output a7_b;
  output [15:0] b0_d;
  output b0_e;
  output b0_v;
  input  b0_b;
  output [15:0] b1_d;
  output b1_e;
  output b1_v;
  input  b1_b;
  output [15:0] b2_d;
  output b2_e;
  output b2_v;
  input  b2_b;
  output [15:0] b3_d;
  output b3_e;
  output b3_v;
  input  b3_b;
  output [15:0] b4_d;
  output b4_e;
  output b4_v;
  input  b4_b;
  output [15:0] b5_d;
  output b5_e;
  output b5_v;
  input  b5_b;
  output [15:0] b6_d;
  output b6_e;
  output b6_v;
  input  b6_b;
  output [15:0] b7_d;
  output b7_e;
  output b7_v;
  input  b7_b;

  wire [4:0] state;
  wire [1:0] statecase;

  tpose_d1_ScOrEtMp47_fsm fsm (clock, reset, a0_e, a0_v, a0_b, a1_e, a1_v, a1_b, a2_e, a2_v, a2_b, a3_e, a3_v, a3_b, a4_e, a4_v, a4_b, a5_e, a5_v, a5_b, a6_e, a6_v, a6_b, a7_e, a7_v, a7_b, b0_e, b0_v, b0_b, b1_e, b1_v, b1_b, b2_e, b2_v, b2_b, b3_e, b3_v, b3_b, b4_e, b4_v, b4_b, b5_e, b5_v, b5_b, b6_e, b6_v, b6_b, b7_e, b7_v, b7_b, state, statecase);
  tpose_d1_ScOrEtMp47_dp dp (clock, reset, a0_d, a1_d, a2_d, a3_d, a4_d, a5_d, a6_d, a7_d, b0_d, b1_d, b2_d, b3_d, b4_d, b5_d, b6_d, b7_d, state, statecase);

endmodule  // tpose_d1_ScOrEtMp47
