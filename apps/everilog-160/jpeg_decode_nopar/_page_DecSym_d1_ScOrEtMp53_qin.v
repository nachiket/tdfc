// Verilog page input queue module for _page_DecSym_d1_ScOrEtMp53
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:44 2009

`include "Q_lowqli_in.v"

module _page_DecSym_d1_ScOrEtMp53_qin (clock, reset, parsToken_qin_d, parsToken_qin_e, parsToken_qin_v, parsToken_qin_b, parsToken_qout_d, parsToken_qout_e, parsToken_qout_v, parsToken_qout_b, ftabval_qin_d, ftabval_qin_e, ftabval_qin_v, ftabval_qin_b, ftabval_qout_d, ftabval_qout_e, ftabval_qout_v, ftabval_qout_b);

  input  clock;
  input  reset;

  input  [15:0] parsToken_qin_d;
  input  parsToken_qin_e;
  input  parsToken_qin_v;
  output parsToken_qin_b;
  output [15:0] parsToken_qout_d;
  output parsToken_qout_e;
  output parsToken_qout_v;
  input  parsToken_qout_b;
  input  [7:0] ftabval_qin_d;
  input  ftabval_qin_e;
  input  ftabval_qin_v;
  output ftabval_qin_b;
  output [7:0] ftabval_qout_d;
  output ftabval_qout_e;
  output ftabval_qout_v;
  input  ftabval_qout_b;

  Q_lowqli_in #(16, 17, 0, 0, 0) q_parsToken (clock, reset, {parsToken_qin_d, parsToken_qin_e}, parsToken_qin_v, parsToken_qin_b, {parsToken_qout_d, parsToken_qout_e}, parsToken_qout_v, parsToken_qout_b);
  Q_lowqli_in #(16, 9, 0, 0, 0) q_ftabval (clock, reset, {ftabval_qin_d, ftabval_qin_e}, ftabval_qin_v, ftabval_qin_b, {ftabval_qout_d, ftabval_qout_e}, ftabval_qout_v, ftabval_qout_b);

endmodule  // _page_DecSym_d1_ScOrEtMp53_qin
