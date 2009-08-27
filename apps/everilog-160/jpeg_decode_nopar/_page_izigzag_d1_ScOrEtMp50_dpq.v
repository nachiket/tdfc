// Verilog page module without input queues or fsms for _page_izigzag_d1_ScOrEtMp50
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:43 2009

`include "_page_izigzag_d1_ScOrEtMp50_q.v"
`include "izigzag_d1_ScOrEtMp50_dp.v"

module izigzag_d1_ScOrEtMp50_fsm (clock, reset, chuA_e, chuA_v, chuA_b, chuB_e, chuB_v, chuB_b, chuC_e, chuC_v, chuC_b, chuD_e, chuD_v, chuD_b, chuE_e, chuE_v, chuE_b, chuF_e, chuF_v, chuF_b, chuG_e, chuG_v, chuG_b, chuH_e, chuH_v, chuH_b, ruS_e, ruS_v, ruS_b, state, statecase, flag__1_0, flag__8_1, flag__8_0, flag__7_1, flag__7_0, flag__6_0, flag__5_1, flag__2_0, flag__1_1, flag__3_0, flag__3_1, flag__4_0, flag__5_0) /* synthesis syn_black_box */ ;

  input  clock;
  input  reset;

  output chuA_e;
  output chuA_v;
  input  chuA_b;
  output chuB_e;
  output chuB_v;
  input  chuB_b;
  output chuC_e;
  output chuC_v;
  input  chuC_b;
  output chuD_e;
  output chuD_v;
  input  chuD_b;
  output chuE_e;
  output chuE_v;
  input  chuE_b;
  output chuF_e;
  output chuF_v;
  input  chuF_b;
  output chuG_e;
  output chuG_v;
  input  chuG_b;
  output chuH_e;
  output chuH_v;
  input  chuH_b;
  input  ruS_e;
  input  ruS_v;
  output ruS_b;

  output [2:0] state;
  output statecase;

  input  flag__1_0;
  input  flag__8_1;
  input  flag__8_0;
  input  flag__7_1;
  input  flag__7_0;
  input  flag__6_0;
  input  flag__5_1;
  input  flag__2_0;
  input  flag__1_1;
  input  flag__3_0;
  input  flag__3_1;
  input  flag__4_0;
  input  flag__5_0;

endmodule  // izigzag_d1_ScOrEtMp50_fsm

module izigzag_d1_ScOrEtMp50 (clock, reset, chuA_d, chuA_e, chuA_v, chuA_b, chuB_d, chuB_e, chuB_v, chuB_b, chuC_d, chuC_e, chuC_v, chuC_b, chuD_d, chuD_e, chuD_v, chuD_b, chuE_d, chuE_e, chuE_v, chuE_b, chuF_d, chuF_e, chuF_v, chuF_b, chuG_d, chuG_e, chuG_v, chuG_b, chuH_d, chuH_e, chuH_v, chuH_b, ruS_d, ruS_e, ruS_v, ruS_b);

  input  clock;
  input  reset;

  output [15:0] chuA_d;
  output chuA_e;
  output chuA_v;
  input  chuA_b;
  output [15:0] chuB_d;
  output chuB_e;
  output chuB_v;
  input  chuB_b;
  output [15:0] chuC_d;
  output chuC_e;
  output chuC_v;
  input  chuC_b;
  output [15:0] chuD_d;
  output chuD_e;
  output chuD_v;
  input  chuD_b;
  output [15:0] chuE_d;
  output chuE_e;
  output chuE_v;
  input  chuE_b;
  output [15:0] chuF_d;
  output chuF_e;
  output chuF_v;
  input  chuF_b;
  output [15:0] chuG_d;
  output chuG_e;
  output chuG_v;
  input  chuG_b;
  output [15:0] chuH_d;
  output chuH_e;
  output chuH_v;
  input  chuH_b;
  input  [15:0] ruS_d;
  input  ruS_e;
  input  ruS_v;
  output ruS_b;

  wire [2:0] state;
  wire statecase;
  wire flag__1_0;
  wire flag__8_1;
  wire flag__8_0;
  wire flag__7_1;
  wire flag__7_0;
  wire flag__6_0;
  wire flag__5_1;
  wire flag__2_0;
  wire flag__1_1;
  wire flag__3_0;
  wire flag__3_1;
  wire flag__4_0;
  wire flag__5_0;

  izigzag_d1_ScOrEtMp50_fsm fsm (clock, reset, chuA_e, chuA_v, chuA_b, chuB_e, chuB_v, chuB_b, chuC_e, chuC_v, chuC_b, chuD_e, chuD_v, chuD_b, chuE_e, chuE_v, chuE_b, chuF_e, chuF_v, chuF_b, chuG_e, chuG_v, chuG_b, chuH_e, chuH_v, chuH_b, ruS_e, ruS_v, ruS_b, state, statecase, flag__1_0, flag__8_1, flag__8_0, flag__7_1, flag__7_0, flag__6_0, flag__5_1, flag__2_0, flag__1_1, flag__3_0, flag__3_1, flag__4_0, flag__5_0);
  izigzag_d1_ScOrEtMp50_dp dp (clock, reset, chuA_d, chuB_d, chuC_d, chuD_d, chuE_d, chuF_d, chuG_d, chuH_d, ruS_d, state, statecase, flag__1_0, flag__8_1, flag__8_0, flag__7_1, flag__7_0, flag__6_0, flag__5_1, flag__2_0, flag__1_1, flag__3_0, flag__3_1, flag__4_0, flag__5_0);

endmodule  // izigzag_d1_ScOrEtMp50

module _page_izigzag_d1_ScOrEtMp50_dpq (clock, reset, chuA_d, chuA_e, chuA_v, chuA_b, chuB_d, chuB_e, chuB_v, chuB_b, chuC_d, chuC_e, chuC_v, chuC_b, chuD_d, chuD_e, chuD_v, chuD_b, chuE_d, chuE_e, chuE_v, chuE_b, chuF_d, chuF_e, chuF_v, chuF_b, chuG_d, chuG_e, chuG_v, chuG_b, chuH_d, chuH_e, chuH_v, chuH_b, ruS_d, ruS_e, ruS_v, ruS_b);

  input  clock;
  input  reset;

  output [15:0] chuA_d;
  output chuA_e;
  output chuA_v;
  input  chuA_b;
  output [15:0] chuB_d;
  output chuB_e;
  output chuB_v;
  input  chuB_b;
  output [15:0] chuC_d;
  output chuC_e;
  output chuC_v;
  input  chuC_b;
  output [15:0] chuD_d;
  output chuD_e;
  output chuD_v;
  input  chuD_b;
  output [15:0] chuE_d;
  output chuE_e;
  output chuE_v;
  input  chuE_b;
  output [15:0] chuF_d;
  output chuF_e;
  output chuF_v;
  input  chuF_b;
  output [15:0] chuG_d;
  output chuG_e;
  output chuG_v;
  input  chuG_b;
  output [15:0] chuH_d;
  output chuH_e;
  output chuH_v;
  input  chuH_b;
  input  [15:0] ruS_d;
  input  ruS_e;
  input  ruS_v;
  output ruS_b;

  wire   [15:0] chuA_qin_d, chuA_qout_d;
  wire   chuA_qin_e, chuA_qout_e;
  wire   chuA_qin_v, chuA_qout_v;
  wire   chuA_qin_b, chuA_qout_b;
  wire   [15:0] chuB_qin_d, chuB_qout_d;
  wire   chuB_qin_e, chuB_qout_e;
  wire   chuB_qin_v, chuB_qout_v;
  wire   chuB_qin_b, chuB_qout_b;
  wire   [15:0] chuC_qin_d, chuC_qout_d;
  wire   chuC_qin_e, chuC_qout_e;
  wire   chuC_qin_v, chuC_qout_v;
  wire   chuC_qin_b, chuC_qout_b;
  wire   [15:0] chuD_qin_d, chuD_qout_d;
  wire   chuD_qin_e, chuD_qout_e;
  wire   chuD_qin_v, chuD_qout_v;
  wire   chuD_qin_b, chuD_qout_b;
  wire   [15:0] chuE_qin_d, chuE_qout_d;
  wire   chuE_qin_e, chuE_qout_e;
  wire   chuE_qin_v, chuE_qout_v;
  wire   chuE_qin_b, chuE_qout_b;
  wire   [15:0] chuF_qin_d, chuF_qout_d;
  wire   chuF_qin_e, chuF_qout_e;
  wire   chuF_qin_v, chuF_qout_v;
  wire   chuF_qin_b, chuF_qout_b;
  wire   [15:0] chuG_qin_d, chuG_qout_d;
  wire   chuG_qin_e, chuG_qout_e;
  wire   chuG_qin_v, chuG_qout_v;
  wire   chuG_qin_b, chuG_qout_b;
  wire   [15:0] chuH_qin_d, chuH_qout_d;
  wire   chuH_qin_e, chuH_qout_e;
  wire   chuH_qin_v, chuH_qout_v;
  wire   chuH_qin_b, chuH_qout_b;
  wire   [15:0] ruS_qin_d, ruS_qout_d;
  wire   ruS_qin_e, ruS_qout_e;
  wire   ruS_qin_v, ruS_qout_v;
  wire   ruS_qin_b, ruS_qout_b;

  assign chuA_d = chuA_qout_d;
  assign chuA_e = chuA_qout_e;
  assign chuA_v = chuA_qout_v;
  assign chuA_qout_b = chuA_b;
  assign chuB_d = chuB_qout_d;
  assign chuB_e = chuB_qout_e;
  assign chuB_v = chuB_qout_v;
  assign chuB_qout_b = chuB_b;
  assign chuC_d = chuC_qout_d;
  assign chuC_e = chuC_qout_e;
  assign chuC_v = chuC_qout_v;
  assign chuC_qout_b = chuC_b;
  assign chuD_d = chuD_qout_d;
  assign chuD_e = chuD_qout_e;
  assign chuD_v = chuD_qout_v;
  assign chuD_qout_b = chuD_b;
  assign chuE_d = chuE_qout_d;
  assign chuE_e = chuE_qout_e;
  assign chuE_v = chuE_qout_v;
  assign chuE_qout_b = chuE_b;
  assign chuF_d = chuF_qout_d;
  assign chuF_e = chuF_qout_e;
  assign chuF_v = chuF_qout_v;
  assign chuF_qout_b = chuF_b;
  assign chuG_d = chuG_qout_d;
  assign chuG_e = chuG_qout_e;
  assign chuG_v = chuG_qout_v;
  assign chuG_qout_b = chuG_b;
  assign chuH_d = chuH_qout_d;
  assign chuH_e = chuH_qout_e;
  assign chuH_v = chuH_qout_v;
  assign chuH_qout_b = chuH_b;
  assign ruS_qout_d = ruS_d;
  assign ruS_qout_e = ruS_e;
  assign ruS_qout_v = ruS_v;
  assign ruS_b = ruS_qout_b;

  _page_izigzag_d1_ScOrEtMp50_q _page_izigzag_d1_ScOrEtMp50_q_ (clock, reset, chuA_qin_d, chuA_qin_e, chuA_qin_v, chuA_qin_b, chuA_qout_d, chuA_qout_e, chuA_qout_v, chuA_qout_b, chuB_qin_d, chuB_qin_e, chuB_qin_v, chuB_qin_b, chuB_qout_d, chuB_qout_e, chuB_qout_v, chuB_qout_b, chuC_qin_d, chuC_qin_e, chuC_qin_v, chuC_qin_b, chuC_qout_d, chuC_qout_e, chuC_qout_v, chuC_qout_b, chuD_qin_d, chuD_qin_e, chuD_qin_v, chuD_qin_b, chuD_qout_d, chuD_qout_e, chuD_qout_v, chuD_qout_b, chuE_qin_d, chuE_qin_e, chuE_qin_v, chuE_qin_b, chuE_qout_d, chuE_qout_e, chuE_qout_v, chuE_qout_b, chuF_qin_d, chuF_qin_e, chuF_qin_v, chuF_qin_b, chuF_qout_d, chuF_qout_e, chuF_qout_v, chuF_qout_b, chuG_qin_d, chuG_qin_e, chuG_qin_v, chuG_qin_b, chuG_qout_d, chuG_qout_e, chuG_qout_v, chuG_qout_b, chuH_qin_d, chuH_qin_e, chuH_qin_v, chuH_qin_b, chuH_qout_d, chuH_qout_e, chuH_qout_v, chuH_qout_b);

  izigzag_d1_ScOrEtMp50 izigzag_d1_ScOrEtMp50_ (clock, reset, chuA_qin_d, chuA_qin_e, chuA_qin_v, chuA_qin_b, chuB_qin_d, chuB_qin_e, chuB_qin_v, chuB_qin_b, chuC_qin_d, chuC_qin_e, chuC_qin_v, chuC_qin_b, chuD_qin_d, chuD_qin_e, chuD_qin_v, chuD_qin_b, chuE_qin_d, chuE_qin_e, chuE_qin_v, chuE_qin_b, chuF_qin_d, chuF_qin_e, chuF_qin_v, chuF_qin_b, chuG_qin_d, chuG_qin_e, chuG_qin_v, chuG_qin_b, chuH_qin_d, chuH_qin_e, chuH_qin_v, chuH_qin_b, ruS_qout_d, ruS_qout_e, ruS_qout_v, ruS_qout_b);

endmodule  // _page_izigzag_d1_ScOrEtMp50_dpq
