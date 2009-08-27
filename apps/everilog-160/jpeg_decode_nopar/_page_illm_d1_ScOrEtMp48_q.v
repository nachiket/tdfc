// Verilog page non-input queue module for _page_illm_d1_ScOrEtMp48
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:43 2009

`include "Q_lowqli.v"
`include "Q_lowqli_out.v"

module _page_illm_d1_ScOrEtMp48_q (clock, reset, b0_qin_d, b0_qin_e, b0_qin_v, b0_qin_b, b0_qout_d, b0_qout_e, b0_qout_v, b0_qout_b, b1_qin_d, b1_qin_e, b1_qin_v, b1_qin_b, b1_qout_d, b1_qout_e, b1_qout_v, b1_qout_b, b2_qin_d, b2_qin_e, b2_qin_v, b2_qin_b, b2_qout_d, b2_qout_e, b2_qout_v, b2_qout_b, b3_qin_d, b3_qin_e, b3_qin_v, b3_qin_b, b3_qout_d, b3_qout_e, b3_qout_v, b3_qout_b, b4_qin_d, b4_qin_e, b4_qin_v, b4_qin_b, b4_qout_d, b4_qout_e, b4_qout_v, b4_qout_b, b5_qin_d, b5_qin_e, b5_qin_v, b5_qin_b, b5_qout_d, b5_qout_e, b5_qout_v, b5_qout_b, b6_qin_d, b6_qin_e, b6_qin_v, b6_qin_b, b6_qout_d, b6_qout_e, b6_qout_v, b6_qout_b, b7_qin_d, b7_qin_e, b7_qin_v, b7_qin_b, b7_qout_d, b7_qout_e, b7_qout_v, b7_qout_b);

  input  clock;
  input  reset;

  input  [8:0] b0_qin_d;
  input  b0_qin_e;
  input  b0_qin_v;
  output b0_qin_b;
  output [8:0] b0_qout_d;
  output b0_qout_e;
  output b0_qout_v;
  input  b0_qout_b;
  input  [8:0] b1_qin_d;
  input  b1_qin_e;
  input  b1_qin_v;
  output b1_qin_b;
  output [8:0] b1_qout_d;
  output b1_qout_e;
  output b1_qout_v;
  input  b1_qout_b;
  input  [8:0] b2_qin_d;
  input  b2_qin_e;
  input  b2_qin_v;
  output b2_qin_b;
  output [8:0] b2_qout_d;
  output b2_qout_e;
  output b2_qout_v;
  input  b2_qout_b;
  input  [8:0] b3_qin_d;
  input  b3_qin_e;
  input  b3_qin_v;
  output b3_qin_b;
  output [8:0] b3_qout_d;
  output b3_qout_e;
  output b3_qout_v;
  input  b3_qout_b;
  input  [8:0] b4_qin_d;
  input  b4_qin_e;
  input  b4_qin_v;
  output b4_qin_b;
  output [8:0] b4_qout_d;
  output b4_qout_e;
  output b4_qout_v;
  input  b4_qout_b;
  input  [8:0] b5_qin_d;
  input  b5_qin_e;
  input  b5_qin_v;
  output b5_qin_b;
  output [8:0] b5_qout_d;
  output b5_qout_e;
  output b5_qout_v;
  input  b5_qout_b;
  input  [8:0] b6_qin_d;
  input  b6_qin_e;
  input  b6_qin_v;
  output b6_qin_b;
  output [8:0] b6_qout_d;
  output b6_qout_e;
  output b6_qout_v;
  input  b6_qout_b;
  input  [8:0] b7_qin_d;
  input  b7_qin_e;
  input  b7_qin_v;
  output b7_qin_b;
  output [8:0] b7_qout_d;
  output b7_qout_e;
  output b7_qout_v;
  input  b7_qout_b;

  Q_lowqli_out #(16, 10, 0, 0, 0) q_b0 (clock, reset, {b0_qin_d, b0_qin_e}, b0_qin_v, b0_qin_b, {b0_qout_d, b0_qout_e}, b0_qout_v, b0_qout_b);
  Q_lowqli_out #(16, 10, 0, 0, 0) q_b1 (clock, reset, {b1_qin_d, b1_qin_e}, b1_qin_v, b1_qin_b, {b1_qout_d, b1_qout_e}, b1_qout_v, b1_qout_b);
  Q_lowqli_out #(16, 10, 0, 0, 0) q_b2 (clock, reset, {b2_qin_d, b2_qin_e}, b2_qin_v, b2_qin_b, {b2_qout_d, b2_qout_e}, b2_qout_v, b2_qout_b);
  Q_lowqli_out #(16, 10, 0, 0, 0) q_b3 (clock, reset, {b3_qin_d, b3_qin_e}, b3_qin_v, b3_qin_b, {b3_qout_d, b3_qout_e}, b3_qout_v, b3_qout_b);
  Q_lowqli_out #(16, 10, 0, 0, 0) q_b4 (clock, reset, {b4_qin_d, b4_qin_e}, b4_qin_v, b4_qin_b, {b4_qout_d, b4_qout_e}, b4_qout_v, b4_qout_b);
  Q_lowqli_out #(16, 10, 0, 0, 0) q_b5 (clock, reset, {b5_qin_d, b5_qin_e}, b5_qin_v, b5_qin_b, {b5_qout_d, b5_qout_e}, b5_qout_v, b5_qout_b);
  Q_lowqli_out #(16, 10, 0, 0, 0) q_b6 (clock, reset, {b6_qin_d, b6_qin_e}, b6_qin_v, b6_qin_b, {b6_qout_d, b6_qout_e}, b6_qout_v, b6_qout_b);
  Q_lowqli_out #(16, 10, 0, 0, 0) q_b7 (clock, reset, {b7_qin_d, b7_qin_e}, b7_qin_v, b7_qin_b, {b7_qout_d, b7_qout_e}, b7_qout_v, b7_qout_b);

endmodule  // _page_illm_d1_ScOrEtMp48_q
