// Verilog FSM module for bl_d1_ScOrEtMp49
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:43 2009

module bl_d1_ScOrEtMp49_fsm (clock, reset, a_e, a_v, a_b, b_e, b_v, b_b, c_e, c_v, c_b, d_e, d_v, d_b, e_e, e_v, e_b, f_e, f_v, f_b, g_e, g_v, g_b, h_e, h_v, h_b, s_e, s_v, s_b, t_e, t_v, t_b, u_e, u_v, u_b, v_e, v_v, v_b, w_e, w_v, w_b, x_e, x_v, x_b, y_e, y_v, y_b, z_e, z_v, z_b, statecase);

  input  clock;
  input  reset;

  input  a_e;
  input  a_v;
  output a_b;
  input  b_e;
  input  b_v;
  output b_b;
  input  c_e;
  input  c_v;
  output c_b;
  input  d_e;
  input  d_v;
  output d_b;
  input  e_e;
  input  e_v;
  output e_b;
  input  f_e;
  input  f_v;
  output f_b;
  input  g_e;
  input  g_v;
  output g_b;
  input  h_e;
  input  h_v;
  output h_b;
  output s_e;
  output s_v;
  input  s_b;
  output t_e;
  output t_v;
  input  t_b;
  output u_e;
  output u_v;
  input  u_b;
  output v_e;
  output v_v;
  input  v_b;
  output w_e;
  output w_v;
  input  w_b;
  output x_e;
  output x_v;
  input  x_b;
  output y_e;
  output y_v;
  input  y_b;
  output z_e;
  output z_v;
  input  z_b;

  output statecase;

  parameter statecase_stall = 1'd0;
  parameter statecase_1 = 1'd1;

  reg statecase_;

  reg a_b_;
  reg b_b_;
  reg c_b_;
  reg d_b_;
  reg e_b_;
  reg f_b_;
  reg g_b_;
  reg h_b_;
  reg s_e_;
  reg s_v_;
  reg t_e_;
  reg t_v_;
  reg u_e_;
  reg u_v_;
  reg v_e_;
  reg v_v_;
  reg w_e_;
  reg w_v_;
  reg x_e_;
  reg x_v_;
  reg y_e_;
  reg y_v_;
  reg z_e_;
  reg z_v_;

  reg did_goto_;

  assign a_b = a_b_;
  assign b_b = b_b_;
  assign c_b = c_b_;
  assign d_b = d_b_;
  assign e_b = e_b_;
  assign f_b = f_b_;
  assign g_b = g_b_;
  assign h_b = h_b_;
  assign s_e = s_e_;
  assign s_v = s_v_;
  assign t_e = t_e_;
  assign t_v = t_v_;
  assign u_e = u_e_;
  assign u_v = u_v_;
  assign v_e = v_e_;
  assign v_v = v_v_;
  assign w_e = w_e_;
  assign w_v = w_v_;
  assign x_e = x_e_;
  assign x_v = x_v_;
  assign y_e = y_e_;
  assign y_v = y_v_;
  assign z_e = z_e_;
  assign z_v = z_v_;
  assign statecase = statecase_;

  always @*  begin
    a_b_ = 1;
    b_b_ = 1;
    c_b_ = 1;
    d_b_ = 1;
    e_b_ = 1;
    f_b_ = 1;
    g_b_ = 1;
    h_b_ = 1;
    s_e_ = 0;
    s_v_ = 0;
    t_e_ = 0;
    t_v_ = 0;
    u_e_ = 0;
    u_v_ = 0;
    v_e_ = 0;
    v_v_ = 0;
    w_e_ = 0;
    w_v_ = 0;
    x_e_ = 0;
    x_v_ = 0;
    y_e_ = 0;
    y_v_ = 0;
    z_e_ = 0;
    z_v_ = 0;

    statecase_ = statecase_stall;
    did_goto_ = 0;

    if (a_v && !a_e && b_v && !b_e && c_v && !c_e && d_v && !d_e && e_v && !e_e && f_v && !f_e && g_v && !g_e && h_v && !h_e && !s_b && !t_b && !u_b && !v_b && !w_b && !x_b && !y_b && !z_b)  begin
      statecase_ = statecase_1;
      a_b_ = 0;
      b_b_ = 0;
      c_b_ = 0;
      d_b_ = 0;
      e_b_ = 0;
      f_b_ = 0;
      g_b_ = 0;
      h_b_ = 0;
      begin
        s_v_ = 1;
        s_e_ = 0;
        t_v_ = 1;
        t_e_ = 0;
        u_v_ = 1;
        u_e_ = 0;
        v_v_ = 1;
        v_e_ = 0;
        w_v_ = 1;
        w_e_ = 0;
        x_v_ = 1;
        x_e_ = 0;
        y_v_ = 1;
        y_e_ = 0;
        z_v_ = 1;
        z_e_ = 0;
      end
    end

  end  // always @*

endmodule  // bl_d1_ScOrEtMp49_fsm
