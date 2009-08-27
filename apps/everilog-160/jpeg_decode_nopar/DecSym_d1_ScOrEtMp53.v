// Verilog top module for DecSym_d1_ScOrEtMp53
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:44 2009

`include "DecSym_d1_ScOrEtMp53_fsm.v"
`include "DecSym_d1_ScOrEtMp53_dp.v"

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
