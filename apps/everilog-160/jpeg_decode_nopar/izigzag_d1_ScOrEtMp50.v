// Verilog top module for izigzag_d1_ScOrEtMp50
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:43 2009

`include "izigzag_d1_ScOrEtMp50_fsm.v"
`include "izigzag_d1_ScOrEtMp50_dp.v"

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
