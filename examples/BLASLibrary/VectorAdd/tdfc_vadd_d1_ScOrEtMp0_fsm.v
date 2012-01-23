// Verilog FSM module for tdfc_vadd_d1_ScOrEtMp0
// Emitted by /home/cody/Internship/tdfc/tdfc version 1.160, Fri May 27 18:31:12 2011

module tdfc_vadd_d1_ScOrEtMp0_fsm (clock, reset, a_e, a_v, a_b, x_e, x_v, x_b, y_e, y_v, y_b, z_e, z_v, z_b, statecase);

  input  clock;
  input  reset;

  input  a_e;
  input  a_v;
  output a_b;
  input  x_e;
  input  x_v;
  output x_b;
  input  y_e;
  input  y_v;
  output y_b;
  output z_e;
  output z_v;
  input  z_b;

  output statecase;

  parameter statecase_stall = 1'd0;
  parameter statecase_1 = 1'd1;

  reg statecase_;

  reg a_b_;
  reg x_b_;
  reg y_b_;
  reg z_e_;
  reg z_v_;

  reg did_goto_;

  assign a_b = a_b_;
  assign x_b = x_b_;
  assign y_b = y_b_;
  assign z_e = z_e_;
  assign z_v = z_v_;
  assign statecase = statecase_;

  always @*  begin
    a_b_ = 1;
    x_b_ = 1;
    y_b_ = 1;
    z_e_ = 0;
    z_v_ = 0;

    statecase_ = statecase_stall;
    did_goto_ = 0;

    if (x_v && !x_e && y_v && !y_e && !z_b)  begin
      statecase_ = statecase_1;
      x_b_ = 0;
      y_b_ = 0;
      begin
        z_v_ = 1;
        z_e_ = 0;
      end
    end

  end  // always @*

endmodule  // tdfc_vadd_d1_ScOrEtMp0_fsm
