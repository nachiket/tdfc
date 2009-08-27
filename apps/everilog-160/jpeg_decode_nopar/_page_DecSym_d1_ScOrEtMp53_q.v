// Verilog page non-input queue module for _page_DecSym_d1_ScOrEtMp53
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:44 2009

`include "Q_lowqli.v"
`include "Q_lowqli_out.v"

module _page_DecSym_d1_ScOrEtMp53_q (clock, reset, setIncr_qin_d, setIncr_qin_e, setIncr_qin_v, setIncr_qin_b, setIncr_qout_d, setIncr_qout_e, setIncr_qout_v, setIncr_qout_b, getReq_qin_d, getReq_qin_e, getReq_qin_v, getReq_qin_b, getReq_qout_d, getReq_qout_e, getReq_qout_v, getReq_qout_b, decToken_qin_d, decToken_qin_e, decToken_qin_v, decToken_qin_b, decToken_qout_d, decToken_qout_e, decToken_qout_v, decToken_qout_b, ftabind_qin_d, ftabind_qin_e, ftabind_qin_v, ftabind_qin_b, ftabind_qout_d, ftabind_qout_e, ftabind_qout_v, ftabind_qout_b);

  input  clock;
  input  reset;

  input  [7:0] setIncr_qin_d;
  input  setIncr_qin_e;
  input  setIncr_qin_v;
  output setIncr_qin_b;
  output [7:0] setIncr_qout_d;
  output setIncr_qout_e;
  output setIncr_qout_v;
  input  setIncr_qout_b;
  input  [7:0] getReq_qin_d;
  input  getReq_qin_e;
  input  getReq_qin_v;
  output getReq_qin_b;
  output [7:0] getReq_qout_d;
  output getReq_qout_e;
  output getReq_qout_v;
  input  getReq_qout_b;
  input  [15:0] decToken_qin_d;
  input  decToken_qin_e;
  input  decToken_qin_v;
  output decToken_qin_b;
  output [15:0] decToken_qout_d;
  output decToken_qout_e;
  output decToken_qout_v;
  input  decToken_qout_b;
  input  [7:0] ftabind_qin_d;
  input  ftabind_qin_e;
  input  ftabind_qin_v;
  output ftabind_qin_b;
  output [7:0] ftabind_qout_d;
  output ftabind_qout_e;
  output ftabind_qout_v;
  input  ftabind_qout_b;

  Q_lowqli_out #(16, 9, 0, 0, 0) q_setIncr (clock, reset, {setIncr_qin_d, setIncr_qin_e}, setIncr_qin_v, setIncr_qin_b, {setIncr_qout_d, setIncr_qout_e}, setIncr_qout_v, setIncr_qout_b);
  Q_lowqli_out #(16, 9, 0, 0, 0) q_getReq (clock, reset, {getReq_qin_d, getReq_qin_e}, getReq_qin_v, getReq_qin_b, {getReq_qout_d, getReq_qout_e}, getReq_qout_v, getReq_qout_b);
  Q_lowqli_out #(16, 17, 0, 0, 0) q_decToken (clock, reset, {decToken_qin_d, decToken_qin_e}, decToken_qin_v, decToken_qin_b, {decToken_qout_d, decToken_qout_e}, decToken_qout_v, decToken_qout_b);
  Q_lowqli_out #(16, 9, 0, 0, 0) q_ftabind (clock, reset, {ftabind_qin_d, ftabind_qin_e}, ftabind_qin_v, ftabind_qin_b, {ftabind_qout_d, ftabind_qout_e}, ftabind_qout_v, ftabind_qout_b);

endmodule  // _page_DecSym_d1_ScOrEtMp53_q
