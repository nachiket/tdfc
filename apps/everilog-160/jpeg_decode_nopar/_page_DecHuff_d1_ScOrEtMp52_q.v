// Verilog page non-input queue module for _page_DecHuff_d1_ScOrEtMp52
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:44 2009

`include "Q_lowqli.v"
`include "Q_lowqli_out.v"

module _page_DecHuff_d1_ScOrEtMp52_q (clock, reset, parsedToken_qin_d, parsedToken_qin_e, parsedToken_qin_v, parsedToken_qin_b, parsedToken_qout_d, parsedToken_qout_e, parsedToken_qout_v, parsedToken_qout_b);

  input  clock;
  input  reset;

  input  [15:0] parsedToken_qin_d;
  input  parsedToken_qin_e;
  input  parsedToken_qin_v;
  output parsedToken_qin_b;
  output [15:0] parsedToken_qout_d;
  output parsedToken_qout_e;
  output parsedToken_qout_v;
  input  parsedToken_qout_b;

  Q_lowqli_out #(16, 17, 0, 0, 0) q_parsedToken (clock, reset, {parsedToken_qin_d, parsedToken_qin_e}, parsedToken_qin_v, parsedToken_qin_b, {parsedToken_qout_d, parsedToken_qout_e}, parsedToken_qout_v, parsedToken_qout_b);

endmodule  // _page_DecHuff_d1_ScOrEtMp52_q
