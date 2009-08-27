// Verilog page non-input queue module for _page_izigzag_d1_ScOrEtMp50
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:43 2009

`include "Q_lowqli.v"
`include "Q_lowqli_out.v"

module _page_izigzag_d1_ScOrEtMp50_q (clock, reset, chuA_qin_d, chuA_qin_e, chuA_qin_v, chuA_qin_b, chuA_qout_d, chuA_qout_e, chuA_qout_v, chuA_qout_b, chuB_qin_d, chuB_qin_e, chuB_qin_v, chuB_qin_b, chuB_qout_d, chuB_qout_e, chuB_qout_v, chuB_qout_b, chuC_qin_d, chuC_qin_e, chuC_qin_v, chuC_qin_b, chuC_qout_d, chuC_qout_e, chuC_qout_v, chuC_qout_b, chuD_qin_d, chuD_qin_e, chuD_qin_v, chuD_qin_b, chuD_qout_d, chuD_qout_e, chuD_qout_v, chuD_qout_b, chuE_qin_d, chuE_qin_e, chuE_qin_v, chuE_qin_b, chuE_qout_d, chuE_qout_e, chuE_qout_v, chuE_qout_b, chuF_qin_d, chuF_qin_e, chuF_qin_v, chuF_qin_b, chuF_qout_d, chuF_qout_e, chuF_qout_v, chuF_qout_b, chuG_qin_d, chuG_qin_e, chuG_qin_v, chuG_qin_b, chuG_qout_d, chuG_qout_e, chuG_qout_v, chuG_qout_b, chuH_qin_d, chuH_qin_e, chuH_qin_v, chuH_qin_b, chuH_qout_d, chuH_qout_e, chuH_qout_v, chuH_qout_b);

  input  clock;
  input  reset;

  input  [15:0] chuA_qin_d;
  input  chuA_qin_e;
  input  chuA_qin_v;
  output chuA_qin_b;
  output [15:0] chuA_qout_d;
  output chuA_qout_e;
  output chuA_qout_v;
  input  chuA_qout_b;
  input  [15:0] chuB_qin_d;
  input  chuB_qin_e;
  input  chuB_qin_v;
  output chuB_qin_b;
  output [15:0] chuB_qout_d;
  output chuB_qout_e;
  output chuB_qout_v;
  input  chuB_qout_b;
  input  [15:0] chuC_qin_d;
  input  chuC_qin_e;
  input  chuC_qin_v;
  output chuC_qin_b;
  output [15:0] chuC_qout_d;
  output chuC_qout_e;
  output chuC_qout_v;
  input  chuC_qout_b;
  input  [15:0] chuD_qin_d;
  input  chuD_qin_e;
  input  chuD_qin_v;
  output chuD_qin_b;
  output [15:0] chuD_qout_d;
  output chuD_qout_e;
  output chuD_qout_v;
  input  chuD_qout_b;
  input  [15:0] chuE_qin_d;
  input  chuE_qin_e;
  input  chuE_qin_v;
  output chuE_qin_b;
  output [15:0] chuE_qout_d;
  output chuE_qout_e;
  output chuE_qout_v;
  input  chuE_qout_b;
  input  [15:0] chuF_qin_d;
  input  chuF_qin_e;
  input  chuF_qin_v;
  output chuF_qin_b;
  output [15:0] chuF_qout_d;
  output chuF_qout_e;
  output chuF_qout_v;
  input  chuF_qout_b;
  input  [15:0] chuG_qin_d;
  input  chuG_qin_e;
  input  chuG_qin_v;
  output chuG_qin_b;
  output [15:0] chuG_qout_d;
  output chuG_qout_e;
  output chuG_qout_v;
  input  chuG_qout_b;
  input  [15:0] chuH_qin_d;
  input  chuH_qin_e;
  input  chuH_qin_v;
  output chuH_qin_b;
  output [15:0] chuH_qout_d;
  output chuH_qout_e;
  output chuH_qout_v;
  input  chuH_qout_b;

  Q_lowqli_out #(16, 17, 0, 0, 0) q_chuA (clock, reset, {chuA_qin_d, chuA_qin_e}, chuA_qin_v, chuA_qin_b, {chuA_qout_d, chuA_qout_e}, chuA_qout_v, chuA_qout_b);
  Q_lowqli_out #(16, 17, 0, 0, 0) q_chuB (clock, reset, {chuB_qin_d, chuB_qin_e}, chuB_qin_v, chuB_qin_b, {chuB_qout_d, chuB_qout_e}, chuB_qout_v, chuB_qout_b);
  Q_lowqli_out #(16, 17, 0, 0, 0) q_chuC (clock, reset, {chuC_qin_d, chuC_qin_e}, chuC_qin_v, chuC_qin_b, {chuC_qout_d, chuC_qout_e}, chuC_qout_v, chuC_qout_b);
  Q_lowqli_out #(16, 17, 0, 0, 0) q_chuD (clock, reset, {chuD_qin_d, chuD_qin_e}, chuD_qin_v, chuD_qin_b, {chuD_qout_d, chuD_qout_e}, chuD_qout_v, chuD_qout_b);
  Q_lowqli_out #(16, 17, 0, 0, 0) q_chuE (clock, reset, {chuE_qin_d, chuE_qin_e}, chuE_qin_v, chuE_qin_b, {chuE_qout_d, chuE_qout_e}, chuE_qout_v, chuE_qout_b);
  Q_lowqli_out #(16, 17, 0, 0, 0) q_chuF (clock, reset, {chuF_qin_d, chuF_qin_e}, chuF_qin_v, chuF_qin_b, {chuF_qout_d, chuF_qout_e}, chuF_qout_v, chuF_qout_b);
  Q_lowqli_out #(16, 17, 0, 0, 0) q_chuG (clock, reset, {chuG_qin_d, chuG_qin_e}, chuG_qin_v, chuG_qin_b, {chuG_qout_d, chuG_qout_e}, chuG_qout_v, chuG_qout_b);
  Q_lowqli_out #(16, 17, 0, 0, 0) q_chuH (clock, reset, {chuH_qin_d, chuH_qin_e}, chuH_qin_v, chuH_qin_b, {chuH_qout_d, chuH_qout_e}, chuH_qout_v, chuH_qout_b);

endmodule  // _page_izigzag_d1_ScOrEtMp50_q
