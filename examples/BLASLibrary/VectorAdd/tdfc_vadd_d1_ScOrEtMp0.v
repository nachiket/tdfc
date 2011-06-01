// Verilog top module for tdfc_vadd_d1_ScOrEtMp0
// Emitted by /home/cody/Internship/tdfc/tdfc version 1.160, Fri May 27 18:31:12 2011

`include "tdfc_vadd_d1_ScOrEtMp0_fsm.v"
`include "tdfc_vadd_d1_ScOrEtMp0_dp.v"

module tdfc_vadd_d1_ScOrEtMp0 (clock, reset, a_d, a_e, a_v, a_b, x_d, x_e, x_v, x_b, y_d, y_e, y_v, y_b, z_d, z_e, z_v, z_b);

  input  clock;
  input  reset;

  input  [7:0] a_d;
  input  a_e;
  input  a_v;
  output a_b;
  input  [7:0] x_d;
  input  x_e;
  input  x_v;
  output x_b;
  input  [7:0] y_d;
  input  y_e;
  input  y_v;
  output y_b;
  output [7:0] z_d;
  output z_e;
  output z_v;
  input  z_b;

  wire statecase;

  tdfc_vadd_d1_ScOrEtMp0_fsm fsm (clock, reset, a_e, a_v, a_b, x_e, x_v, x_b, y_e, y_v, y_b, z_e, z_v, z_b, statecase);
  tdfc_vadd_d1_ScOrEtMp0_dp dp (clock, reset, a_d, x_d, y_d, z_d, statecase);

endmodule  // tdfc_vadd_d1_ScOrEtMp0
