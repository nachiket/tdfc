// Verilog page input queue module for _page_DecHuff_d1_ScOrEtMp52
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:44 2009

`include "Q_lowqli_in.v"

module _page_DecHuff_d1_ScOrEtMp52_qin (clock, reset, filebyte_qin_d, filebyte_qin_e, filebyte_qin_v, filebyte_qin_b, filebyte_qout_d, filebyte_qout_e, filebyte_qout_v, filebyte_qout_b, reqSize_qin_d, reqSize_qin_e, reqSize_qin_v, reqSize_qin_b, reqSize_qout_d, reqSize_qout_e, reqSize_qout_v, reqSize_qout_b, advance_qin_d, advance_qin_e, advance_qin_v, advance_qin_b, advance_qout_d, advance_qout_e, advance_qout_v, advance_qout_b);

  input  clock;
  input  reset;

  input  [7:0] filebyte_qin_d;
  input  filebyte_qin_e;
  input  filebyte_qin_v;
  output filebyte_qin_b;
  output [7:0] filebyte_qout_d;
  output filebyte_qout_e;
  output filebyte_qout_v;
  input  filebyte_qout_b;
  input  [7:0] reqSize_qin_d;
  input  reqSize_qin_e;
  input  reqSize_qin_v;
  output reqSize_qin_b;
  output [7:0] reqSize_qout_d;
  output reqSize_qout_e;
  output reqSize_qout_v;
  input  reqSize_qout_b;
  input  [7:0] advance_qin_d;
  input  advance_qin_e;
  input  advance_qin_v;
  output advance_qin_b;
  output [7:0] advance_qout_d;
  output advance_qout_e;
  output advance_qout_v;
  input  advance_qout_b;

  Q_lowqli_in #(16, 9, 0, 0, 0) q_filebyte (clock, reset, {filebyte_qin_d, filebyte_qin_e}, filebyte_qin_v, filebyte_qin_b, {filebyte_qout_d, filebyte_qout_e}, filebyte_qout_v, filebyte_qout_b);
  Q_lowqli_in #(16, 9, 0, 0, 0) q_reqSize (clock, reset, {reqSize_qin_d, reqSize_qin_e}, reqSize_qin_v, reqSize_qin_b, {reqSize_qout_d, reqSize_qout_e}, reqSize_qout_v, reqSize_qout_b);
  Q_lowqli_in #(16, 9, 0, 0, 0) q_advance (clock, reset, {advance_qin_d, advance_qin_e}, advance_qin_v, advance_qin_b, {advance_qout_d, advance_qout_e}, advance_qout_v, advance_qout_b);

endmodule  // _page_DecHuff_d1_ScOrEtMp52_qin
