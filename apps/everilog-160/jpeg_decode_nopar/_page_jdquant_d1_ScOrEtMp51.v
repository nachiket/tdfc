// Verilog page module for _page_jdquant_d1_ScOrEtMp51
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:44 2009

`include "_page_jdquant_d1_ScOrEtMp51_qin.v"
`include "_page_jdquant_d1_ScOrEtMp51_noin.v"

module _page_jdquant_d1_ScOrEtMp51 (clock, reset, inStream_d, inStream_e, inStream_v, inStream_b, outStream_d, outStream_e, outStream_v, outStream_b) /* synthesis syn_hier="flatten,firm" */ ;

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

  wire   [15:0] inStream_qin_d, inStream_qout_d;
  wire   inStream_qin_e, inStream_qout_e;
  wire   inStream_qin_v, inStream_qout_v;
  wire   inStream_qin_b, inStream_qout_b;
  wire   [15:0] outStream_qin_d, outStream_qout_d;
  wire   outStream_qin_e, outStream_qout_e;
  wire   outStream_qin_v, outStream_qout_v;
  wire   outStream_qin_b, outStream_qout_b;

  assign inStream_qin_d = inStream_d;
  assign inStream_qin_e = inStream_e;
  assign inStream_qin_v = inStream_v;
  assign inStream_b = inStream_qin_b;
  assign outStream_d = outStream_qout_d;
  assign outStream_e = outStream_qout_e;
  assign outStream_v = outStream_qout_v;
  assign outStream_qout_b = outStream_b;

  _page_jdquant_d1_ScOrEtMp51_qin qin (clock, reset, inStream_qin_d, inStream_qin_e, inStream_qin_v, inStream_qin_b, inStream_qout_d, inStream_qout_e, inStream_qout_v, inStream_qout_b);

  _page_jdquant_d1_ScOrEtMp51_noin noin (clock, reset, inStream_qout_d, inStream_qout_e, inStream_qout_v, inStream_qout_b, outStream_qout_d, outStream_qout_e, outStream_qout_v, outStream_qout_b);

endmodule  // _page_jdquant_d1_ScOrEtMp51
