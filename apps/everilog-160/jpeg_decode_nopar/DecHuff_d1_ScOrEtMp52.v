// Verilog top module for DecHuff_d1_ScOrEtMp52
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:44 2009

`include "DecHuff_d1_ScOrEtMp52_fsm.v"
`include "DecHuff_d1_ScOrEtMp52_dp.v"

module DecHuff_d1_ScOrEtMp52 (clock, reset, filebyte_d, filebyte_e, filebyte_v, filebyte_b, reqSize_d, reqSize_e, reqSize_v, reqSize_b, advance_d, advance_e, advance_v, advance_b, parsedToken_d, parsedToken_e, parsedToken_v, parsedToken_b);

  input  clock;
  input  reset;

  input  [7:0] filebyte_d;
  input  filebyte_e;
  input  filebyte_v;
  output filebyte_b;
  input  [7:0] reqSize_d;
  input  reqSize_e;
  input  reqSize_v;
  output reqSize_b;
  input  [7:0] advance_d;
  input  advance_e;
  input  advance_v;
  output advance_b;
  output [15:0] parsedToken_d;
  output parsedToken_e;
  output parsedToken_v;
  input  parsedToken_b;

  wire [2:0] state;
  wire statecase;
  wire flag_getData_0;
  wire flag_waitZero_0;
  wire flag_getData_1;
  wire flag_req_0;

  DecHuff_d1_ScOrEtMp52_fsm fsm (clock, reset, filebyte_e, filebyte_v, filebyte_b, reqSize_e, reqSize_v, reqSize_b, advance_e, advance_v, advance_b, parsedToken_e, parsedToken_v, parsedToken_b, state, statecase, flag_getData_0, flag_waitZero_0, flag_getData_1, flag_req_0);
  DecHuff_d1_ScOrEtMp52_dp dp (clock, reset, filebyte_d, reqSize_d, advance_d, parsedToken_d, state, statecase, flag_getData_0, flag_waitZero_0, flag_getData_1, flag_req_0);

endmodule  // DecHuff_d1_ScOrEtMp52
