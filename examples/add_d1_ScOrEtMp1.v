// Verilog top module for add_d1_ScOrEtMp1
// Emitted by ../tdfc version 1.160, Thu Aug 27 19:13:22 2009

`include "add_d1_ScOrEtMp1_fsm.v"
`include "add_d1_ScOrEtMp1_dp.v"

module add_d1_ScOrEtMp1 (clock, reset, add_d1_ScOrEtMp1_d, add_d1_ScOrEtMp1_e, add_d1_ScOrEtMp1_v, add_d1_ScOrEtMp1_b, a_d, a_e, a_v, a_b, b_d, b_e, b_v, b_b);

  input  clock;
  input  reset;

  output add_d1_ScOrEtMp1_d;
  output add_d1_ScOrEtMp1_e;
  output add_d1_ScOrEtMp1_v;
  input  add_d1_ScOrEtMp1_b;
  input  a_d;
  input  a_e;
  input  a_v;
  output a_b;
  input  b_d;
  input  b_e;
  input  b_v;
  output b_b;

  wire statecase;

  add_d1_ScOrEtMp1_fsm fsm (clock, reset, add_d1_ScOrEtMp1_e, add_d1_ScOrEtMp1_v, add_d1_ScOrEtMp1_b, a_e, a_v, a_b, b_e, b_v, b_b, statecase);
  add_d1_ScOrEtMp1_dp dp (clock, reset, add_d1_ScOrEtMp1_d, a_d, b_d, statecase);

endmodule  // add_d1_ScOrEtMp1
