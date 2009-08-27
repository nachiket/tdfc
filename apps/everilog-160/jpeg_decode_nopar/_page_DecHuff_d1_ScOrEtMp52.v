// Verilog page module for _page_DecHuff_d1_ScOrEtMp52
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:44 2009

`include "_page_DecHuff_d1_ScOrEtMp52_qin.v"
`include "_page_DecHuff_d1_ScOrEtMp52_noin.v"

module _page_DecHuff_d1_ScOrEtMp52 (clock, reset, filebyte_d, filebyte_e, filebyte_v, filebyte_b, reqSize_d, reqSize_e, reqSize_v, reqSize_b, advance_d, advance_e, advance_v, advance_b, parsedToken_d, parsedToken_e, parsedToken_v, parsedToken_b) /* synthesis syn_hier="flatten,firm" */ ;

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

  assign filebyte_qin_d = filebyte_d;
  assign filebyte_qin_e = filebyte_e;
  assign filebyte_qin_v = filebyte_v;
  assign filebyte_b = filebyte_qin_b;
  assign reqSize_qin_d = reqSize_d;
  assign reqSize_qin_e = reqSize_e;
  assign reqSize_qin_v = reqSize_v;
  assign reqSize_b = reqSize_qin_b;
  assign advance_qin_d = advance_d;
  assign advance_qin_e = advance_e;
  assign advance_qin_v = advance_v;
  assign advance_b = advance_qin_b;
  assign parsedToken_d = parsedToken_qout_d;
  assign parsedToken_e = parsedToken_qout_e;
  assign parsedToken_v = parsedToken_qout_v;
  assign parsedToken_qout_b = parsedToken_b;

  _page_DecHuff_d1_ScOrEtMp52_qin qin (clock, reset, filebyte_qin_d, filebyte_qin_e, filebyte_qin_v, filebyte_qin_b, filebyte_qout_d, filebyte_qout_e, filebyte_qout_v, filebyte_qout_b, reqSize_qin_d, reqSize_qin_e, reqSize_qin_v, reqSize_qin_b, reqSize_qout_d, reqSize_qout_e, reqSize_qout_v, reqSize_qout_b, advance_qin_d, advance_qin_e, advance_qin_v, advance_qin_b, advance_qout_d, advance_qout_e, advance_qout_v, advance_qout_b);

  _page_DecHuff_d1_ScOrEtMp52_noin noin (clock, reset, filebyte_qout_d, filebyte_qout_e, filebyte_qout_v, filebyte_qout_b, reqSize_qout_d, reqSize_qout_e, reqSize_qout_v, reqSize_qout_b, advance_qout_d, advance_qout_e, advance_qout_v, advance_qout_b, parsedToken_qout_d, parsedToken_qout_e, parsedToken_qout_v, parsedToken_qout_b);

endmodule  // _page_DecHuff_d1_ScOrEtMp52
