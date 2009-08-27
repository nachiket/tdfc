// Verilog page module without input queues for _page_bl_d1_ScOrEtMp49
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:43 2009

`include "_page_bl_d1_ScOrEtMp49_q.v"
`include "bl_d1_ScOrEtMp49.v"

module _page_bl_d1_ScOrEtMp49_noin (clock, reset, a_d, a_e, a_v, a_b, b_d, b_e, b_v, b_b, c_d, c_e, c_v, c_b, d_d, d_e, d_v, d_b, e_d, e_e, e_v, e_b, f_d, f_e, f_v, f_b, g_d, g_e, g_v, g_b, h_d, h_e, h_v, h_b, s_d, s_e, s_v, s_b, t_d, t_e, t_v, t_b, u_d, u_e, u_v, u_b, v_d, v_e, v_v, v_b, w_d, w_e, w_v, w_b, x_d, x_e, x_v, x_b, y_d, y_e, y_v, y_b, z_d, z_e, z_v, z_b);

  input  clock;
  input  reset;

  input  [8:0] a_d;
  input  a_e;
  input  a_v;
  output a_b;
  input  [8:0] b_d;
  input  b_e;
  input  b_v;
  output b_b;
  input  [8:0] c_d;
  input  c_e;
  input  c_v;
  output c_b;
  input  [8:0] d_d;
  input  d_e;
  input  d_v;
  output d_b;
  input  [8:0] e_d;
  input  e_e;
  input  e_v;
  output e_b;
  input  [8:0] f_d;
  input  f_e;
  input  f_v;
  output f_b;
  input  [8:0] g_d;
  input  g_e;
  input  g_v;
  output g_b;
  input  [8:0] h_d;
  input  h_e;
  input  h_v;
  output h_b;
  output [8:0] s_d;
  output s_e;
  output s_v;
  input  s_b;
  output [8:0] t_d;
  output t_e;
  output t_v;
  input  t_b;
  output [8:0] u_d;
  output u_e;
  output u_v;
  input  u_b;
  output [8:0] v_d;
  output v_e;
  output v_v;
  input  v_b;
  output [8:0] w_d;
  output w_e;
  output w_v;
  input  w_b;
  output [8:0] x_d;
  output x_e;
  output x_v;
  input  x_b;
  output [8:0] y_d;
  output y_e;
  output y_v;
  input  y_b;
  output [8:0] z_d;
  output z_e;
  output z_v;
  input  z_b;

  wire   [8:0] a_qin_d, a_qout_d;
  wire   a_qin_e, a_qout_e;
  wire   a_qin_v, a_qout_v;
  wire   a_qin_b, a_qout_b;
  wire   [8:0] b_qin_d, b_qout_d;
  wire   b_qin_e, b_qout_e;
  wire   b_qin_v, b_qout_v;
  wire   b_qin_b, b_qout_b;
  wire   [8:0] c_qin_d, c_qout_d;
  wire   c_qin_e, c_qout_e;
  wire   c_qin_v, c_qout_v;
  wire   c_qin_b, c_qout_b;
  wire   [8:0] d_qin_d, d_qout_d;
  wire   d_qin_e, d_qout_e;
  wire   d_qin_v, d_qout_v;
  wire   d_qin_b, d_qout_b;
  wire   [8:0] e_qin_d, e_qout_d;
  wire   e_qin_e, e_qout_e;
  wire   e_qin_v, e_qout_v;
  wire   e_qin_b, e_qout_b;
  wire   [8:0] f_qin_d, f_qout_d;
  wire   f_qin_e, f_qout_e;
  wire   f_qin_v, f_qout_v;
  wire   f_qin_b, f_qout_b;
  wire   [8:0] g_qin_d, g_qout_d;
  wire   g_qin_e, g_qout_e;
  wire   g_qin_v, g_qout_v;
  wire   g_qin_b, g_qout_b;
  wire   [8:0] h_qin_d, h_qout_d;
  wire   h_qin_e, h_qout_e;
  wire   h_qin_v, h_qout_v;
  wire   h_qin_b, h_qout_b;
  wire   [8:0] s_qin_d, s_qout_d;
  wire   s_qin_e, s_qout_e;
  wire   s_qin_v, s_qout_v;
  wire   s_qin_b, s_qout_b;
  wire   [8:0] t_qin_d, t_qout_d;
  wire   t_qin_e, t_qout_e;
  wire   t_qin_v, t_qout_v;
  wire   t_qin_b, t_qout_b;
  wire   [8:0] u_qin_d, u_qout_d;
  wire   u_qin_e, u_qout_e;
  wire   u_qin_v, u_qout_v;
  wire   u_qin_b, u_qout_b;
  wire   [8:0] v_qin_d, v_qout_d;
  wire   v_qin_e, v_qout_e;
  wire   v_qin_v, v_qout_v;
  wire   v_qin_b, v_qout_b;
  wire   [8:0] w_qin_d, w_qout_d;
  wire   w_qin_e, w_qout_e;
  wire   w_qin_v, w_qout_v;
  wire   w_qin_b, w_qout_b;
  wire   [8:0] x_qin_d, x_qout_d;
  wire   x_qin_e, x_qout_e;
  wire   x_qin_v, x_qout_v;
  wire   x_qin_b, x_qout_b;
  wire   [8:0] y_qin_d, y_qout_d;
  wire   y_qin_e, y_qout_e;
  wire   y_qin_v, y_qout_v;
  wire   y_qin_b, y_qout_b;
  wire   [8:0] z_qin_d, z_qout_d;
  wire   z_qin_e, z_qout_e;
  wire   z_qin_v, z_qout_v;
  wire   z_qin_b, z_qout_b;

  assign a_qout_d = a_d;
  assign a_qout_e = a_e;
  assign a_qout_v = a_v;
  assign a_b = a_qout_b;
  assign b_qout_d = b_d;
  assign b_qout_e = b_e;
  assign b_qout_v = b_v;
  assign b_b = b_qout_b;
  assign c_qout_d = c_d;
  assign c_qout_e = c_e;
  assign c_qout_v = c_v;
  assign c_b = c_qout_b;
  assign d_qout_d = d_d;
  assign d_qout_e = d_e;
  assign d_qout_v = d_v;
  assign d_b = d_qout_b;
  assign e_qout_d = e_d;
  assign e_qout_e = e_e;
  assign e_qout_v = e_v;
  assign e_b = e_qout_b;
  assign f_qout_d = f_d;
  assign f_qout_e = f_e;
  assign f_qout_v = f_v;
  assign f_b = f_qout_b;
  assign g_qout_d = g_d;
  assign g_qout_e = g_e;
  assign g_qout_v = g_v;
  assign g_b = g_qout_b;
  assign h_qout_d = h_d;
  assign h_qout_e = h_e;
  assign h_qout_v = h_v;
  assign h_b = h_qout_b;
  assign s_d = s_qout_d;
  assign s_e = s_qout_e;
  assign s_v = s_qout_v;
  assign s_qout_b = s_b;
  assign t_d = t_qout_d;
  assign t_e = t_qout_e;
  assign t_v = t_qout_v;
  assign t_qout_b = t_b;
  assign u_d = u_qout_d;
  assign u_e = u_qout_e;
  assign u_v = u_qout_v;
  assign u_qout_b = u_b;
  assign v_d = v_qout_d;
  assign v_e = v_qout_e;
  assign v_v = v_qout_v;
  assign v_qout_b = v_b;
  assign w_d = w_qout_d;
  assign w_e = w_qout_e;
  assign w_v = w_qout_v;
  assign w_qout_b = w_b;
  assign x_d = x_qout_d;
  assign x_e = x_qout_e;
  assign x_v = x_qout_v;
  assign x_qout_b = x_b;
  assign y_d = y_qout_d;
  assign y_e = y_qout_e;
  assign y_v = y_qout_v;
  assign y_qout_b = y_b;
  assign z_d = z_qout_d;
  assign z_e = z_qout_e;
  assign z_v = z_qout_v;
  assign z_qout_b = z_b;

  _page_bl_d1_ScOrEtMp49_q _page_bl_d1_ScOrEtMp49_q_ (clock, reset, s_qin_d, s_qin_e, s_qin_v, s_qin_b, s_qout_d, s_qout_e, s_qout_v, s_qout_b, t_qin_d, t_qin_e, t_qin_v, t_qin_b, t_qout_d, t_qout_e, t_qout_v, t_qout_b, u_qin_d, u_qin_e, u_qin_v, u_qin_b, u_qout_d, u_qout_e, u_qout_v, u_qout_b, v_qin_d, v_qin_e, v_qin_v, v_qin_b, v_qout_d, v_qout_e, v_qout_v, v_qout_b, w_qin_d, w_qin_e, w_qin_v, w_qin_b, w_qout_d, w_qout_e, w_qout_v, w_qout_b, x_qin_d, x_qin_e, x_qin_v, x_qin_b, x_qout_d, x_qout_e, x_qout_v, x_qout_b, y_qin_d, y_qin_e, y_qin_v, y_qin_b, y_qout_d, y_qout_e, y_qout_v, y_qout_b, z_qin_d, z_qin_e, z_qin_v, z_qin_b, z_qout_d, z_qout_e, z_qout_v, z_qout_b);

  bl_d1_ScOrEtMp49 bl_d1_ScOrEtMp49_ (clock, reset, a_qout_d, a_qout_e, a_qout_v, a_qout_b, b_qout_d, b_qout_e, b_qout_v, b_qout_b, c_qout_d, c_qout_e, c_qout_v, c_qout_b, d_qout_d, d_qout_e, d_qout_v, d_qout_b, e_qout_d, e_qout_e, e_qout_v, e_qout_b, f_qout_d, f_qout_e, f_qout_v, f_qout_b, g_qout_d, g_qout_e, g_qout_v, g_qout_b, h_qout_d, h_qout_e, h_qout_v, h_qout_b, s_qin_d, s_qin_e, s_qin_v, s_qin_b, t_qin_d, t_qin_e, t_qin_v, t_qin_b, u_qin_d, u_qin_e, u_qin_v, u_qin_b, v_qin_d, v_qin_e, v_qin_v, v_qin_b, w_qin_d, w_qin_e, w_qin_v, w_qin_b, x_qin_d, x_qin_e, x_qin_v, x_qin_b, y_qin_d, y_qin_e, y_qin_v, y_qin_b, z_qin_d, z_qin_e, z_qin_v, z_qin_b);

endmodule  // _page_bl_d1_ScOrEtMp49_noin
