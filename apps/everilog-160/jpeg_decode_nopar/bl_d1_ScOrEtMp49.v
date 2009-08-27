// Verilog top module for bl_d1_ScOrEtMp49
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:43 2009

`include "bl_d1_ScOrEtMp49_fsm.v"
`include "bl_d1_ScOrEtMp49_dp.v"

module bl_d1_ScOrEtMp49 (clock, reset, a_d, a_e, a_v, a_b, b_d, b_e, b_v, b_b, c_d, c_e, c_v, c_b, d_d, d_e, d_v, d_b, e_d, e_e, e_v, e_b, f_d, f_e, f_v, f_b, g_d, g_e, g_v, g_b, h_d, h_e, h_v, h_b, s_d, s_e, s_v, s_b, t_d, t_e, t_v, t_b, u_d, u_e, u_v, u_b, v_d, v_e, v_v, v_b, w_d, w_e, w_v, w_b, x_d, x_e, x_v, x_b, y_d, y_e, y_v, y_b, z_d, z_e, z_v, z_b);

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

  wire statecase;

  bl_d1_ScOrEtMp49_fsm fsm (clock, reset, a_e, a_v, a_b, b_e, b_v, b_b, c_e, c_v, c_b, d_e, d_v, d_b, e_e, e_v, e_b, f_e, f_v, f_b, g_e, g_v, g_b, h_e, h_v, h_b, s_e, s_v, s_b, t_e, t_v, t_b, u_e, u_v, u_b, v_e, v_v, v_b, w_e, w_v, w_b, x_e, x_v, x_b, y_e, y_v, y_b, z_e, z_v, z_b, statecase);
  bl_d1_ScOrEtMp49_dp dp (clock, reset, a_d, b_d, c_d, d_d, e_d, f_d, g_d, h_d, s_d, t_d, u_d, v_d, w_d, x_d, y_d, z_d, statecase);

endmodule  // bl_d1_ScOrEtMp49
