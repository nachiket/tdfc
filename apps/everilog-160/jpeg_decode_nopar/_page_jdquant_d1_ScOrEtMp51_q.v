// Verilog page non-input queue module for _page_jdquant_d1_ScOrEtMp51
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:43 2009

`include "Q_lowqli.v"
`include "Q_lowqli_out.v"

module _page_jdquant_d1_ScOrEtMp51_q (clock, reset, outStream_qin_d, outStream_qin_e, outStream_qin_v, outStream_qin_b, outStream_qout_d, outStream_qout_e, outStream_qout_v, outStream_qout_b);

  input  clock;
  input  reset;

  input  [15:0] outStream_qin_d;
  input  outStream_qin_e;
  input  outStream_qin_v;
  output outStream_qin_b;
  output [15:0] outStream_qout_d;
  output outStream_qout_e;
  output outStream_qout_v;
  input  outStream_qout_b;

  Q_lowqli_out #(16, 17, 0, 0, 0) q_outStream (clock, reset, {outStream_qin_d, outStream_qin_e}, outStream_qin_v, outStream_qin_b, {outStream_qout_d, outStream_qout_e}, outStream_qout_v, outStream_qout_b);

endmodule  // _page_jdquant_d1_ScOrEtMp51_q
