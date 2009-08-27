// Verilog top module for jdquant_d1_ScOrEtMp51
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:44 2009

`include "jdquant_d1_ScOrEtMp51_fsm.v"
`include "jdquant_d1_ScOrEtMp51_dp.v"

module jdquant_d1_ScOrEtMp51 (clock, reset, inStream_d, inStream_e, inStream_v, inStream_b, outStream_d, outStream_e, outStream_v, outStream_b);

  input  clock;
  input  reset;

  input  [15:0] inStream_d;
  input  inStream_e;
  input  inStream_v;
  output inStream_b;
  output [15:0] outStream_d;
  output outStream_e;
  output outStream_v;
  input  outStream_b;

  wire statecase;
  wire flag_only_0;

  jdquant_d1_ScOrEtMp51_fsm fsm (clock, reset, inStream_e, inStream_v, inStream_b, outStream_e, outStream_v, outStream_b, statecase, flag_only_0);
  jdquant_d1_ScOrEtMp51_dp dp (clock, reset, inStream_d, outStream_d, statecase, flag_only_0);

endmodule  // jdquant_d1_ScOrEtMp51
