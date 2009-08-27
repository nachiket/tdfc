// Verilog page input queue module for _page_bl_d1_ScOrEtMp49
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:43 2009

`include "Q_lowqli_in.v"

module _page_bl_d1_ScOrEtMp49_qin (clock, reset, a_qin_d, a_qin_e, a_qin_v, a_qin_b, a_qout_d, a_qout_e, a_qout_v, a_qout_b, b_qin_d, b_qin_e, b_qin_v, b_qin_b, b_qout_d, b_qout_e, b_qout_v, b_qout_b, c_qin_d, c_qin_e, c_qin_v, c_qin_b, c_qout_d, c_qout_e, c_qout_v, c_qout_b, d_qin_d, d_qin_e, d_qin_v, d_qin_b, d_qout_d, d_qout_e, d_qout_v, d_qout_b, e_qin_d, e_qin_e, e_qin_v, e_qin_b, e_qout_d, e_qout_e, e_qout_v, e_qout_b, f_qin_d, f_qin_e, f_qin_v, f_qin_b, f_qout_d, f_qout_e, f_qout_v, f_qout_b, g_qin_d, g_qin_e, g_qin_v, g_qin_b, g_qout_d, g_qout_e, g_qout_v, g_qout_b, h_qin_d, h_qin_e, h_qin_v, h_qin_b, h_qout_d, h_qout_e, h_qout_v, h_qout_b);

  input  clock;
  input  reset;

  input  [8:0] a_qin_d;
  input  a_qin_e;
  input  a_qin_v;
  output a_qin_b;
  output [8:0] a_qout_d;
  output a_qout_e;
  output a_qout_v;
  input  a_qout_b;
  input  [8:0] b_qin_d;
  input  b_qin_e;
  input  b_qin_v;
  output b_qin_b;
  output [8:0] b_qout_d;
  output b_qout_e;
  output b_qout_v;
  input  b_qout_b;
  input  [8:0] c_qin_d;
  input  c_qin_e;
  input  c_qin_v;
  output c_qin_b;
  output [8:0] c_qout_d;
  output c_qout_e;
  output c_qout_v;
  input  c_qout_b;
  input  [8:0] d_qin_d;
  input  d_qin_e;
  input  d_qin_v;
  output d_qin_b;
  output [8:0] d_qout_d;
  output d_qout_e;
  output d_qout_v;
  input  d_qout_b;
  input  [8:0] e_qin_d;
  input  e_qin_e;
  input  e_qin_v;
  output e_qin_b;
  output [8:0] e_qout_d;
  output e_qout_e;
  output e_qout_v;
  input  e_qout_b;
  input  [8:0] f_qin_d;
  input  f_qin_e;
  input  f_qin_v;
  output f_qin_b;
  output [8:0] f_qout_d;
  output f_qout_e;
  output f_qout_v;
  input  f_qout_b;
  input  [8:0] g_qin_d;
  input  g_qin_e;
  input  g_qin_v;
  output g_qin_b;
  output [8:0] g_qout_d;
  output g_qout_e;
  output g_qout_v;
  input  g_qout_b;
  input  [8:0] h_qin_d;
  input  h_qin_e;
  input  h_qin_v;
  output h_qin_b;
  output [8:0] h_qout_d;
  output h_qout_e;
  output h_qout_v;
  input  h_qout_b;

  Q_lowqli_in #(16, 10, 0, 0, 0) q_a (clock, reset, {a_qin_d, a_qin_e}, a_qin_v, a_qin_b, {a_qout_d, a_qout_e}, a_qout_v, a_qout_b);
  Q_lowqli_in #(16, 10, 0, 0, 0) q_b (clock, reset, {b_qin_d, b_qin_e}, b_qin_v, b_qin_b, {b_qout_d, b_qout_e}, b_qout_v, b_qout_b);
  Q_lowqli_in #(16, 10, 0, 0, 0) q_c (clock, reset, {c_qin_d, c_qin_e}, c_qin_v, c_qin_b, {c_qout_d, c_qout_e}, c_qout_v, c_qout_b);
  Q_lowqli_in #(16, 10, 0, 0, 0) q_d (clock, reset, {d_qin_d, d_qin_e}, d_qin_v, d_qin_b, {d_qout_d, d_qout_e}, d_qout_v, d_qout_b);
  Q_lowqli_in #(16, 10, 0, 0, 0) q_e (clock, reset, {e_qin_d, e_qin_e}, e_qin_v, e_qin_b, {e_qout_d, e_qout_e}, e_qout_v, e_qout_b);
  Q_lowqli_in #(16, 10, 0, 0, 0) q_f (clock, reset, {f_qin_d, f_qin_e}, f_qin_v, f_qin_b, {f_qout_d, f_qout_e}, f_qout_v, f_qout_b);
  Q_lowqli_in #(16, 10, 0, 0, 0) q_g (clock, reset, {g_qin_d, g_qin_e}, g_qin_v, g_qin_b, {g_qout_d, g_qout_e}, g_qout_v, g_qout_b);
  Q_lowqli_in #(16, 10, 0, 0, 0) q_h (clock, reset, {h_qin_d, h_qin_e}, h_qin_v, h_qin_b, {h_qout_d, h_qout_e}, h_qout_v, h_qout_b);

endmodule  // _page_bl_d1_ScOrEtMp49_qin
