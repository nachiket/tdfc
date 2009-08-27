// Verilog page input queue module for _page_illm_d1_ScOrEtMp48
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:43 2009

`include "Q_lowqli_in.v"

module _page_illm_d1_ScOrEtMp48_qin (clock, reset, a0_qin_d, a0_qin_e, a0_qin_v, a0_qin_b, a0_qout_d, a0_qout_e, a0_qout_v, a0_qout_b, a1_qin_d, a1_qin_e, a1_qin_v, a1_qin_b, a1_qout_d, a1_qout_e, a1_qout_v, a1_qout_b, a2_qin_d, a2_qin_e, a2_qin_v, a2_qin_b, a2_qout_d, a2_qout_e, a2_qout_v, a2_qout_b, a3_qin_d, a3_qin_e, a3_qin_v, a3_qin_b, a3_qout_d, a3_qout_e, a3_qout_v, a3_qout_b, a4_qin_d, a4_qin_e, a4_qin_v, a4_qin_b, a4_qout_d, a4_qout_e, a4_qout_v, a4_qout_b, a5_qin_d, a5_qin_e, a5_qin_v, a5_qin_b, a5_qout_d, a5_qout_e, a5_qout_v, a5_qout_b, a6_qin_d, a6_qin_e, a6_qin_v, a6_qin_b, a6_qout_d, a6_qout_e, a6_qout_v, a6_qout_b, a7_qin_d, a7_qin_e, a7_qin_v, a7_qin_b, a7_qout_d, a7_qout_e, a7_qout_v, a7_qout_b);

  input  clock;
  input  reset;

  input  [15:0] a0_qin_d;
  input  a0_qin_e;
  input  a0_qin_v;
  output a0_qin_b;
  output [15:0] a0_qout_d;
  output a0_qout_e;
  output a0_qout_v;
  input  a0_qout_b;
  input  [15:0] a1_qin_d;
  input  a1_qin_e;
  input  a1_qin_v;
  output a1_qin_b;
  output [15:0] a1_qout_d;
  output a1_qout_e;
  output a1_qout_v;
  input  a1_qout_b;
  input  [15:0] a2_qin_d;
  input  a2_qin_e;
  input  a2_qin_v;
  output a2_qin_b;
  output [15:0] a2_qout_d;
  output a2_qout_e;
  output a2_qout_v;
  input  a2_qout_b;
  input  [15:0] a3_qin_d;
  input  a3_qin_e;
  input  a3_qin_v;
  output a3_qin_b;
  output [15:0] a3_qout_d;
  output a3_qout_e;
  output a3_qout_v;
  input  a3_qout_b;
  input  [15:0] a4_qin_d;
  input  a4_qin_e;
  input  a4_qin_v;
  output a4_qin_b;
  output [15:0] a4_qout_d;
  output a4_qout_e;
  output a4_qout_v;
  input  a4_qout_b;
  input  [15:0] a5_qin_d;
  input  a5_qin_e;
  input  a5_qin_v;
  output a5_qin_b;
  output [15:0] a5_qout_d;
  output a5_qout_e;
  output a5_qout_v;
  input  a5_qout_b;
  input  [15:0] a6_qin_d;
  input  a6_qin_e;
  input  a6_qin_v;
  output a6_qin_b;
  output [15:0] a6_qout_d;
  output a6_qout_e;
  output a6_qout_v;
  input  a6_qout_b;
  input  [15:0] a7_qin_d;
  input  a7_qin_e;
  input  a7_qin_v;
  output a7_qin_b;
  output [15:0] a7_qout_d;
  output a7_qout_e;
  output a7_qout_v;
  input  a7_qout_b;

  Q_lowqli_in #(16, 17, 0, 0, 0) q_a0 (clock, reset, {a0_qin_d, a0_qin_e}, a0_qin_v, a0_qin_b, {a0_qout_d, a0_qout_e}, a0_qout_v, a0_qout_b);
  Q_lowqli_in #(16, 17, 0, 0, 0) q_a1 (clock, reset, {a1_qin_d, a1_qin_e}, a1_qin_v, a1_qin_b, {a1_qout_d, a1_qout_e}, a1_qout_v, a1_qout_b);
  Q_lowqli_in #(16, 17, 0, 0, 0) q_a2 (clock, reset, {a2_qin_d, a2_qin_e}, a2_qin_v, a2_qin_b, {a2_qout_d, a2_qout_e}, a2_qout_v, a2_qout_b);
  Q_lowqli_in #(16, 17, 0, 0, 0) q_a3 (clock, reset, {a3_qin_d, a3_qin_e}, a3_qin_v, a3_qin_b, {a3_qout_d, a3_qout_e}, a3_qout_v, a3_qout_b);
  Q_lowqli_in #(16, 17, 0, 0, 0) q_a4 (clock, reset, {a4_qin_d, a4_qin_e}, a4_qin_v, a4_qin_b, {a4_qout_d, a4_qout_e}, a4_qout_v, a4_qout_b);
  Q_lowqli_in #(16, 17, 0, 0, 0) q_a5 (clock, reset, {a5_qin_d, a5_qin_e}, a5_qin_v, a5_qin_b, {a5_qout_d, a5_qout_e}, a5_qout_v, a5_qout_b);
  Q_lowqli_in #(16, 17, 0, 0, 0) q_a6 (clock, reset, {a6_qin_d, a6_qin_e}, a6_qin_v, a6_qin_b, {a6_qout_d, a6_qout_e}, a6_qout_v, a6_qout_b);
  Q_lowqli_in #(16, 17, 0, 0, 0) q_a7 (clock, reset, {a7_qin_d, a7_qin_e}, a7_qin_v, a7_qin_b, {a7_qout_d, a7_qout_e}, a7_qout_v, a7_qout_b);

endmodule  // _page_illm_d1_ScOrEtMp48_qin
