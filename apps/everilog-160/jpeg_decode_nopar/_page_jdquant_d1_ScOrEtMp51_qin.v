// Verilog page input queue module for _page_jdquant_d1_ScOrEtMp51
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:43 2009

`include "Q_lowqli_in.v"

module _page_jdquant_d1_ScOrEtMp51_qin (clock, reset, inStream_qin_d, inStream_qin_e, inStream_qin_v, inStream_qin_b, inStream_qout_d, inStream_qout_e, inStream_qout_v, inStream_qout_b);

  input  clock;
  input  reset;

  input  [15:0] inStream_qin_d;
  input  inStream_qin_e;
  input  inStream_qin_v;
  output inStream_qin_b;
  output [15:0] inStream_qout_d;
  output inStream_qout_e;
  output inStream_qout_v;
  input  inStream_qout_b;

  Q_lowqli_in #(16, 17, 0, 0, 0) q_inStream (clock, reset, {inStream_qin_d, inStream_qin_e}, inStream_qin_v, inStream_qin_b, {inStream_qout_d, inStream_qout_e}, inStream_qout_v, inStream_qout_b);

endmodule  // _page_jdquant_d1_ScOrEtMp51_qin
