// Verilog page non-input queue module for _page_bl_d1_ScOrEtMp49
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:43 2009

`include "Q_lowqli.v"
`include "Q_lowqli_out.v"

module _page_bl_d1_ScOrEtMp49_q (clock, reset, s_qin_d, s_qin_e, s_qin_v, s_qin_b, s_qout_d, s_qout_e, s_qout_v, s_qout_b, t_qin_d, t_qin_e, t_qin_v, t_qin_b, t_qout_d, t_qout_e, t_qout_v, t_qout_b, u_qin_d, u_qin_e, u_qin_v, u_qin_b, u_qout_d, u_qout_e, u_qout_v, u_qout_b, v_qin_d, v_qin_e, v_qin_v, v_qin_b, v_qout_d, v_qout_e, v_qout_v, v_qout_b, w_qin_d, w_qin_e, w_qin_v, w_qin_b, w_qout_d, w_qout_e, w_qout_v, w_qout_b, x_qin_d, x_qin_e, x_qin_v, x_qin_b, x_qout_d, x_qout_e, x_qout_v, x_qout_b, y_qin_d, y_qin_e, y_qin_v, y_qin_b, y_qout_d, y_qout_e, y_qout_v, y_qout_b, z_qin_d, z_qin_e, z_qin_v, z_qin_b, z_qout_d, z_qout_e, z_qout_v, z_qout_b);

  input  clock;
  input  reset;

  input  [8:0] s_qin_d;
  input  s_qin_e;
  input  s_qin_v;
  output s_qin_b;
  output [8:0] s_qout_d;
  output s_qout_e;
  output s_qout_v;
  input  s_qout_b;
  input  [8:0] t_qin_d;
  input  t_qin_e;
  input  t_qin_v;
  output t_qin_b;
  output [8:0] t_qout_d;
  output t_qout_e;
  output t_qout_v;
  input  t_qout_b;
  input  [8:0] u_qin_d;
  input  u_qin_e;
  input  u_qin_v;
  output u_qin_b;
  output [8:0] u_qout_d;
  output u_qout_e;
  output u_qout_v;
  input  u_qout_b;
  input  [8:0] v_qin_d;
  input  v_qin_e;
  input  v_qin_v;
  output v_qin_b;
  output [8:0] v_qout_d;
  output v_qout_e;
  output v_qout_v;
  input  v_qout_b;
  input  [8:0] w_qin_d;
  input  w_qin_e;
  input  w_qin_v;
  output w_qin_b;
  output [8:0] w_qout_d;
  output w_qout_e;
  output w_qout_v;
  input  w_qout_b;
  input  [8:0] x_qin_d;
  input  x_qin_e;
  input  x_qin_v;
  output x_qin_b;
  output [8:0] x_qout_d;
  output x_qout_e;
  output x_qout_v;
  input  x_qout_b;
  input  [8:0] y_qin_d;
  input  y_qin_e;
  input  y_qin_v;
  output y_qin_b;
  output [8:0] y_qout_d;
  output y_qout_e;
  output y_qout_v;
  input  y_qout_b;
  input  [8:0] z_qin_d;
  input  z_qin_e;
  input  z_qin_v;
  output z_qin_b;
  output [8:0] z_qout_d;
  output z_qout_e;
  output z_qout_v;
  input  z_qout_b;

  Q_lowqli_out #(16, 10, 0, 0, 0) q_s (clock, reset, {s_qin_d, s_qin_e}, s_qin_v, s_qin_b, {s_qout_d, s_qout_e}, s_qout_v, s_qout_b);
  Q_lowqli_out #(16, 10, 0, 0, 0) q_t (clock, reset, {t_qin_d, t_qin_e}, t_qin_v, t_qin_b, {t_qout_d, t_qout_e}, t_qout_v, t_qout_b);
  Q_lowqli_out #(16, 10, 0, 0, 0) q_u (clock, reset, {u_qin_d, u_qin_e}, u_qin_v, u_qin_b, {u_qout_d, u_qout_e}, u_qout_v, u_qout_b);
  Q_lowqli_out #(16, 10, 0, 0, 0) q_v (clock, reset, {v_qin_d, v_qin_e}, v_qin_v, v_qin_b, {v_qout_d, v_qout_e}, v_qout_v, v_qout_b);
  Q_lowqli_out #(16, 10, 0, 0, 0) q_w (clock, reset, {w_qin_d, w_qin_e}, w_qin_v, w_qin_b, {w_qout_d, w_qout_e}, w_qout_v, w_qout_b);
  Q_lowqli_out #(16, 10, 0, 0, 0) q_x (clock, reset, {x_qin_d, x_qin_e}, x_qin_v, x_qin_b, {x_qout_d, x_qout_e}, x_qout_v, x_qout_b);
  Q_lowqli_out #(16, 10, 0, 0, 0) q_y (clock, reset, {y_qin_d, y_qin_e}, y_qin_v, y_qin_b, {y_qout_d, y_qout_e}, y_qout_v, y_qout_b);
  Q_lowqli_out #(16, 10, 0, 0, 0) q_z (clock, reset, {z_qin_d, z_qin_e}, z_qin_v, z_qin_b, {z_qout_d, z_qout_e}, z_qout_v, z_qout_b);

endmodule  // _page_bl_d1_ScOrEtMp49_q
