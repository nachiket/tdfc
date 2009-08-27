// Verilog page module without input queues or fsms for _page_DecSym_d1_ScOrEtMp53
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:44 2009

`include "_page_DecSym_d1_ScOrEtMp53_q.v"
`include "DecSym_d1_ScOrEtMp53_dp.v"

module DecSym_d1_ScOrEtMp53_fsm (clock, reset, parsToken_e, parsToken_v, parsToken_b, setIncr_e, setIncr_v, setIncr_b, getReq_e, getReq_v, getReq_b, decToken_e, decToken_v, decToken_b, ftabind_e, ftabind_v, ftabind_b, ftabval_e, ftabval_v, ftabval_b, state, statecase, flag_DecAC1_0, flag_spitZero_3, flag_spitZero_2, flag_spitZero_1, flag_spitZero_0, flag_DecRS__0, flag_DecRS_1, flag_DecRS_0, flag_DecACLen_0, flag_DecAC2_0, flag_DecDC_0, flag_DecDC_1, flag_DecDC_2, flag_DecDCLen_0) /* synthesis syn_black_box */ ;

  input  clock;
  input  reset;

  input  parsToken_e;
  input  parsToken_v;
  output parsToken_b;
  output setIncr_e;
  output setIncr_v;
  input  setIncr_b;
  output getReq_e;
  output getReq_v;
  input  getReq_b;
  output decToken_e;
  output decToken_v;
  input  decToken_b;
  output ftabind_e;
  output ftabind_v;
  input  ftabind_b;
  input  ftabval_e;
  input  ftabval_v;
  output ftabval_b;

  output [3:0] state;
  output statecase;

  input  flag_DecAC1_0;
  input  flag_spitZero_3;
  input  flag_spitZero_2;
  input  flag_spitZero_1;
  input  flag_spitZero_0;
  input  flag_DecRS__0;
  input  flag_DecRS_1;
  input  flag_DecRS_0;
  input  flag_DecACLen_0;
  input  flag_DecAC2_0;
  input  flag_DecDC_0;
  input  flag_DecDC_1;
  input  flag_DecDC_2;
  input  flag_DecDCLen_0;

endmodule  // DecSym_d1_ScOrEtMp53_fsm

module DecSym_d1_ScOrEtMp53 (clock, reset, parsToken_d, parsToken_e, parsToken_v, parsToken_b, setIncr_d, setIncr_e, setIncr_v, setIncr_b, getReq_d, getReq_e, getReq_v, getReq_b, decToken_d, decToken_e, decToken_v, decToken_b, ftabind_d, ftabind_e, ftabind_v, ftabind_b, ftabval_d, ftabval_e, ftabval_v, ftabval_b);

  input  clock;
  input  reset;

  input  [15:0] parsToken_d;
  input  parsToken_e;
  input  parsToken_v;
  output parsToken_b;
  output [7:0] setIncr_d;
  output setIncr_e;
  output setIncr_v;
  input  setIncr_b;
  output [7:0] getReq_d;
  output getReq_e;
  output getReq_v;
  input  getReq_b;
  output [15:0] decToken_d;
  output decToken_e;
  output decToken_v;
  input  decToken_b;
  output [7:0] ftabind_d;
  output ftabind_e;
  output ftabind_v;
  input  ftabind_b;
  input  [7:0] ftabval_d;
  input  ftabval_e;
  input  ftabval_v;
  output ftabval_b;

  wire [3:0] state;
  wire statecase;
  wire flag_DecAC1_0;
  wire flag_spitZero_3;
  wire flag_spitZero_2;
  wire flag_spitZero_1;
  wire flag_spitZero_0;
  wire flag_DecRS__0;
  wire flag_DecRS_1;
  wire flag_DecRS_0;
  wire flag_DecACLen_0;
  wire flag_DecAC2_0;
  wire flag_DecDC_0;
  wire flag_DecDC_1;
  wire flag_DecDC_2;
  wire flag_DecDCLen_0;

  DecSym_d1_ScOrEtMp53_fsm fsm (clock, reset, parsToken_e, parsToken_v, parsToken_b, setIncr_e, setIncr_v, setIncr_b, getReq_e, getReq_v, getReq_b, decToken_e, decToken_v, decToken_b, ftabind_e, ftabind_v, ftabind_b, ftabval_e, ftabval_v, ftabval_b, state, statecase, flag_DecAC1_0, flag_spitZero_3, flag_spitZero_2, flag_spitZero_1, flag_spitZero_0, flag_DecRS__0, flag_DecRS_1, flag_DecRS_0, flag_DecACLen_0, flag_DecAC2_0, flag_DecDC_0, flag_DecDC_1, flag_DecDC_2, flag_DecDCLen_0);
  DecSym_d1_ScOrEtMp53_dp dp (clock, reset, parsToken_d, setIncr_d, getReq_d, decToken_d, ftabind_d, ftabval_d, state, statecase, flag_DecAC1_0, flag_spitZero_3, flag_spitZero_2, flag_spitZero_1, flag_spitZero_0, flag_DecRS__0, flag_DecRS_1, flag_DecRS_0, flag_DecACLen_0, flag_DecAC2_0, flag_DecDC_0, flag_DecDC_1, flag_DecDC_2, flag_DecDCLen_0);

endmodule  // DecSym_d1_ScOrEtMp53

module _page_DecSym_d1_ScOrEtMp53_dpq (clock, reset, parsToken_d, parsToken_e, parsToken_v, parsToken_b, setIncr_d, setIncr_e, setIncr_v, setIncr_b, getReq_d, getReq_e, getReq_v, getReq_b, decToken_d, decToken_e, decToken_v, decToken_b, ftabind_d, ftabind_e, ftabind_v, ftabind_b, ftabval_d, ftabval_e, ftabval_v, ftabval_b);

  input  clock;
  input  reset;

  input  [15:0] parsToken_d;
  input  parsToken_e;
  input  parsToken_v;
  output parsToken_b;
  output [7:0] setIncr_d;
  output setIncr_e;
  output setIncr_v;
  input  setIncr_b;
  output [7:0] getReq_d;
  output getReq_e;
  output getReq_v;
  input  getReq_b;
  output [15:0] decToken_d;
  output decToken_e;
  output decToken_v;
  input  decToken_b;
  output [7:0] ftabind_d;
  output ftabind_e;
  output ftabind_v;
  input  ftabind_b;
  input  [7:0] ftabval_d;
  input  ftabval_e;
  input  ftabval_v;
  output ftabval_b;

  wire   [15:0] parsToken_qin_d, parsToken_qout_d;
  wire   parsToken_qin_e, parsToken_qout_e;
  wire   parsToken_qin_v, parsToken_qout_v;
  wire   parsToken_qin_b, parsToken_qout_b;
  wire   [7:0] setIncr_qin_d, setIncr_qout_d;
  wire   setIncr_qin_e, setIncr_qout_e;
  wire   setIncr_qin_v, setIncr_qout_v;
  wire   setIncr_qin_b, setIncr_qout_b;
  wire   [7:0] getReq_qin_d, getReq_qout_d;
  wire   getReq_qin_e, getReq_qout_e;
  wire   getReq_qin_v, getReq_qout_v;
  wire   getReq_qin_b, getReq_qout_b;
  wire   [15:0] decToken_qin_d, decToken_qout_d;
  wire   decToken_qin_e, decToken_qout_e;
  wire   decToken_qin_v, decToken_qout_v;
  wire   decToken_qin_b, decToken_qout_b;
  wire   [7:0] ftabind_qin_d, ftabind_qout_d;
  wire   ftabind_qin_e, ftabind_qout_e;
  wire   ftabind_qin_v, ftabind_qout_v;
  wire   ftabind_qin_b, ftabind_qout_b;
  wire   [7:0] ftabval_qin_d, ftabval_qout_d;
  wire   ftabval_qin_e, ftabval_qout_e;
  wire   ftabval_qin_v, ftabval_qout_v;
  wire   ftabval_qin_b, ftabval_qout_b;

  assign parsToken_qout_d = parsToken_d;
  assign parsToken_qout_e = parsToken_e;
  assign parsToken_qout_v = parsToken_v;
  assign parsToken_b = parsToken_qout_b;
  assign setIncr_d = setIncr_qout_d;
  assign setIncr_e = setIncr_qout_e;
  assign setIncr_v = setIncr_qout_v;
  assign setIncr_qout_b = setIncr_b;
  assign getReq_d = getReq_qout_d;
  assign getReq_e = getReq_qout_e;
  assign getReq_v = getReq_qout_v;
  assign getReq_qout_b = getReq_b;
  assign decToken_d = decToken_qout_d;
  assign decToken_e = decToken_qout_e;
  assign decToken_v = decToken_qout_v;
  assign decToken_qout_b = decToken_b;
  assign ftabind_d = ftabind_qout_d;
  assign ftabind_e = ftabind_qout_e;
  assign ftabind_v = ftabind_qout_v;
  assign ftabind_qout_b = ftabind_b;
  assign ftabval_qout_d = ftabval_d;
  assign ftabval_qout_e = ftabval_e;
  assign ftabval_qout_v = ftabval_v;
  assign ftabval_b = ftabval_qout_b;

  _page_DecSym_d1_ScOrEtMp53_q _page_DecSym_d1_ScOrEtMp53_q_ (clock, reset, setIncr_qin_d, setIncr_qin_e, setIncr_qin_v, setIncr_qin_b, setIncr_qout_d, setIncr_qout_e, setIncr_qout_v, setIncr_qout_b, getReq_qin_d, getReq_qin_e, getReq_qin_v, getReq_qin_b, getReq_qout_d, getReq_qout_e, getReq_qout_v, getReq_qout_b, decToken_qin_d, decToken_qin_e, decToken_qin_v, decToken_qin_b, decToken_qout_d, decToken_qout_e, decToken_qout_v, decToken_qout_b, ftabind_qin_d, ftabind_qin_e, ftabind_qin_v, ftabind_qin_b, ftabind_qout_d, ftabind_qout_e, ftabind_qout_v, ftabind_qout_b);

  DecSym_d1_ScOrEtMp53 DecSym_d1_ScOrEtMp53_ (clock, reset, parsToken_qout_d, parsToken_qout_e, parsToken_qout_v, parsToken_qout_b, setIncr_qin_d, setIncr_qin_e, setIncr_qin_v, setIncr_qin_b, getReq_qin_d, getReq_qin_e, getReq_qin_v, getReq_qin_b, decToken_qin_d, decToken_qin_e, decToken_qin_v, decToken_qin_b, ftabind_qin_d, ftabind_qin_e, ftabind_qin_v, ftabind_qin_b, ftabval_qout_d, ftabval_qout_e, ftabval_qout_v, ftabval_qout_b);

endmodule  // _page_DecSym_d1_ScOrEtMp53_dpq
