// Verilog page module without input queues or fsms for _page_DecHuff_d1_ScOrEtMp52
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:44 2009

`include "_page_DecHuff_d1_ScOrEtMp52_q.v"
`include "DecHuff_d1_ScOrEtMp52_dp.v"

module DecHuff_d1_ScOrEtMp52_fsm (clock, reset, filebyte_e, filebyte_v, filebyte_b, reqSize_e, reqSize_v, reqSize_b, advance_e, advance_v, advance_b, parsedToken_e, parsedToken_v, parsedToken_b, state, statecase, flag_getData_0, flag_waitZero_0, flag_getData_1, flag_req_0) /* synthesis syn_black_box */ ;

  input  clock;
  input  reset;

  input  filebyte_e;
  input  filebyte_v;
  output filebyte_b;
  input  reqSize_e;
  input  reqSize_v;
  output reqSize_b;
  input  advance_e;
  input  advance_v;
  output advance_b;
  output parsedToken_e;
  output parsedToken_v;
  input  parsedToken_b;

  output [2:0] state;
  output statecase;

  input  flag_getData_0;
  input  flag_waitZero_0;
  input  flag_getData_1;
  input  flag_req_0;

endmodule  // DecHuff_d1_ScOrEtMp52_fsm

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

module _page_DecHuff_d1_ScOrEtMp52_dpq (clock, reset, filebyte_d, filebyte_e, filebyte_v, filebyte_b, reqSize_d, reqSize_e, reqSize_v, reqSize_b, advance_d, advance_e, advance_v, advance_b, parsedToken_d, parsedToken_e, parsedToken_v, parsedToken_b);

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

  wire   [7:0] filebyte_qin_d, filebyte_qout_d;
  wire   filebyte_qin_e, filebyte_qout_e;
  wire   filebyte_qin_v, filebyte_qout_v;
  wire   filebyte_qin_b, filebyte_qout_b;
  wire   [7:0] reqSize_qin_d, reqSize_qout_d;
  wire   reqSize_qin_e, reqSize_qout_e;
  wire   reqSize_qin_v, reqSize_qout_v;
  wire   reqSize_qin_b, reqSize_qout_b;
  wire   [7:0] advance_qin_d, advance_qout_d;
  wire   advance_qin_e, advance_qout_e;
  wire   advance_qin_v, advance_qout_v;
  wire   advance_qin_b, advance_qout_b;
  wire   [15:0] parsedToken_qin_d, parsedToken_qout_d;
  wire   parsedToken_qin_e, parsedToken_qout_e;
  wire   parsedToken_qin_v, parsedToken_qout_v;
  wire   parsedToken_qin_b, parsedToken_qout_b;

  assign filebyte_qout_d = filebyte_d;
  assign filebyte_qout_e = filebyte_e;
  assign filebyte_qout_v = filebyte_v;
  assign filebyte_b = filebyte_qout_b;
  assign reqSize_qout_d = reqSize_d;
  assign reqSize_qout_e = reqSize_e;
  assign reqSize_qout_v = reqSize_v;
  assign reqSize_b = reqSize_qout_b;
  assign advance_qout_d = advance_d;
  assign advance_qout_e = advance_e;
  assign advance_qout_v = advance_v;
  assign advance_b = advance_qout_b;
  assign parsedToken_d = parsedToken_qout_d;
  assign parsedToken_e = parsedToken_qout_e;
  assign parsedToken_v = parsedToken_qout_v;
  assign parsedToken_qout_b = parsedToken_b;

  _page_DecHuff_d1_ScOrEtMp52_q _page_DecHuff_d1_ScOrEtMp52_q_ (clock, reset, parsedToken_qin_d, parsedToken_qin_e, parsedToken_qin_v, parsedToken_qin_b, parsedToken_qout_d, parsedToken_qout_e, parsedToken_qout_v, parsedToken_qout_b);

  DecHuff_d1_ScOrEtMp52 DecHuff_d1_ScOrEtMp52_ (clock, reset, filebyte_qout_d, filebyte_qout_e, filebyte_qout_v, filebyte_qout_b, reqSize_qout_d, reqSize_qout_e, reqSize_qout_v, reqSize_qout_b, advance_qout_d, advance_qout_e, advance_qout_v, advance_qout_b, parsedToken_qin_d, parsedToken_qin_e, parsedToken_qin_v, parsedToken_qin_b);

endmodule  // _page_DecHuff_d1_ScOrEtMp52_dpq
