// Verilog FSM module for illm_d1_ScOrEtMp46
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:43 2009

module illm_d1_ScOrEtMp46_fsm (clock, reset, a0_e, a0_v, a0_b, a1_e, a1_v, a1_b, a2_e, a2_v, a2_b, a3_e, a3_v, a3_b, a4_e, a4_v, a4_b, a5_e, a5_v, a5_b, a6_e, a6_v, a6_b, a7_e, a7_v, a7_b, b0_e, b0_v, b0_b, b1_e, b1_v, b1_b, b2_e, b2_v, b2_b, b3_e, b3_v, b3_b, b4_e, b4_v, b4_b, b5_e, b5_v, b5_b, b6_e, b6_v, b6_b, b7_e, b7_v, b7_b, statecase);

  input  clock;
  input  reset;

  input  a0_e;
  input  a0_v;
  output a0_b;
  input  a1_e;
  input  a1_v;
  output a1_b;
  input  a2_e;
  input  a2_v;
  output a2_b;
  input  a3_e;
  input  a3_v;
  output a3_b;
  input  a4_e;
  input  a4_v;
  output a4_b;
  input  a5_e;
  input  a5_v;
  output a5_b;
  input  a6_e;
  input  a6_v;
  output a6_b;
  input  a7_e;
  input  a7_v;
  output a7_b;
  output b0_e;
  output b0_v;
  input  b0_b;
  output b1_e;
  output b1_v;
  input  b1_b;
  output b2_e;
  output b2_v;
  input  b2_b;
  output b3_e;
  output b3_v;
  input  b3_b;
  output b4_e;
  output b4_v;
  input  b4_b;
  output b5_e;
  output b5_v;
  input  b5_b;
  output b6_e;
  output b6_v;
  input  b6_b;
  output b7_e;
  output b7_v;
  input  b7_b;

  output statecase;

  parameter statecase_stall = 1'd0;
  parameter statecase_1 = 1'd1;

  reg statecase_;

  reg a0_b_;
  reg a1_b_;
  reg a2_b_;
  reg a3_b_;
  reg a4_b_;
  reg a5_b_;
  reg a6_b_;
  reg a7_b_;
  reg b0_e_;
  reg b0_v_;
  reg b1_e_;
  reg b1_v_;
  reg b2_e_;
  reg b2_v_;
  reg b3_e_;
  reg b3_v_;
  reg b4_e_;
  reg b4_v_;
  reg b5_e_;
  reg b5_v_;
  reg b6_e_;
  reg b6_v_;
  reg b7_e_;
  reg b7_v_;

  reg did_goto_;

  assign a0_b = a0_b_;
  assign a1_b = a1_b_;
  assign a2_b = a2_b_;
  assign a3_b = a3_b_;
  assign a4_b = a4_b_;
  assign a5_b = a5_b_;
  assign a6_b = a6_b_;
  assign a7_b = a7_b_;
  assign b0_e = b0_e_;
  assign b0_v = b0_v_;
  assign b1_e = b1_e_;
  assign b1_v = b1_v_;
  assign b2_e = b2_e_;
  assign b2_v = b2_v_;
  assign b3_e = b3_e_;
  assign b3_v = b3_v_;
  assign b4_e = b4_e_;
  assign b4_v = b4_v_;
  assign b5_e = b5_e_;
  assign b5_v = b5_v_;
  assign b6_e = b6_e_;
  assign b6_v = b6_v_;
  assign b7_e = b7_e_;
  assign b7_v = b7_v_;
  assign statecase = statecase_;

  always @*  begin
    a0_b_ = 1;
    a1_b_ = 1;
    a2_b_ = 1;
    a3_b_ = 1;
    a4_b_ = 1;
    a5_b_ = 1;
    a6_b_ = 1;
    a7_b_ = 1;
    b0_e_ = 0;
    b0_v_ = 0;
    b1_e_ = 0;
    b1_v_ = 0;
    b2_e_ = 0;
    b2_v_ = 0;
    b3_e_ = 0;
    b3_v_ = 0;
    b4_e_ = 0;
    b4_v_ = 0;
    b5_e_ = 0;
    b5_v_ = 0;
    b6_e_ = 0;
    b6_v_ = 0;
    b7_e_ = 0;
    b7_v_ = 0;

    statecase_ = statecase_stall;
    did_goto_ = 0;

    if (a0_v && !a0_e && a1_v && !a1_e && a2_v && !a2_e && a3_v && !a3_e && a4_v && !a4_e && a5_v && !a5_e && a6_v && !a6_e && a7_v && !a7_e && !b0_b && !b1_b && !b2_b && !b3_b && !b4_b && !b5_b && !b6_b && !b7_b)  begin
      statecase_ = statecase_1;
      a0_b_ = 0;
      a1_b_ = 0;
      a2_b_ = 0;
      a3_b_ = 0;
      a4_b_ = 0;
      a5_b_ = 0;
      a6_b_ = 0;
      a7_b_ = 0;
      begin
        b0_v_ = 1;
        b0_e_ = 0;
        b7_v_ = 1;
        b7_e_ = 0;
        b1_v_ = 1;
        b1_e_ = 0;
        b6_v_ = 1;
        b6_e_ = 0;
        b2_v_ = 1;
        b2_e_ = 0;
        b5_v_ = 1;
        b5_e_ = 0;
        b4_v_ = 1;
        b4_e_ = 0;
        b3_v_ = 1;
        b3_e_ = 0;
        did_goto_ = 1;
      end
    end

  end  // always @*

endmodule  // illm_d1_ScOrEtMp46_fsm
