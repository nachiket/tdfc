// Verilog FSM module for add_d1_ScOrEtMp1
// Emitted by ../tdfc version 1.160, Thu Aug 27 19:13:22 2009

module add_d1_ScOrEtMp1_fsm (clock, reset, add_d1_ScOrEtMp1_e, add_d1_ScOrEtMp1_v, add_d1_ScOrEtMp1_b, a_e, a_v, a_b, b_e, b_v, b_b, statecase);

  input  clock;
  input  reset;

  output add_d1_ScOrEtMp1_e;
  output add_d1_ScOrEtMp1_v;
  input  add_d1_ScOrEtMp1_b;
  input  a_e;
  input  a_v;
  output a_b;
  input  b_e;
  input  b_v;
  output b_b;

  output statecase;

  parameter statecase_stall = 1'd0;
  parameter statecase_1 = 1'd1;

  reg statecase_;

  reg add_d1_ScOrEtMp1_e_;
  reg add_d1_ScOrEtMp1_v_;
  reg a_b_;
  reg b_b_;

  reg did_goto_;

  assign add_d1_ScOrEtMp1_e = add_d1_ScOrEtMp1_e_;
  assign add_d1_ScOrEtMp1_v = add_d1_ScOrEtMp1_v_;
  assign a_b = a_b_;
  assign b_b = b_b_;
  assign statecase = statecase_;

  always @*  begin
    add_d1_ScOrEtMp1_e_ = 0;
    add_d1_ScOrEtMp1_v_ = 0;
    a_b_ = 1;
    b_b_ = 1;

    statecase_ = statecase_stall;
    did_goto_ = 0;

    if (a_v && !a_e && b_v && !b_e && !add_d1_ScOrEtMp1_b)  begin
      statecase_ = statecase_1;
      a_b_ = 0;
      b_b_ = 0;
      add_d1_ScOrEtMp1_v_ = 1;
      add_d1_ScOrEtMp1_e_ = 0;
    end

  end  // always @*

endmodule  // add_d1_ScOrEtMp1_fsm
