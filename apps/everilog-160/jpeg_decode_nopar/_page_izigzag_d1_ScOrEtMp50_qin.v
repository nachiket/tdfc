// Verilog page input queue module for _page_izigzag_d1_ScOrEtMp50
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:43 2009

`include "Q_lowqli_in.v"

module _page_izigzag_d1_ScOrEtMp50_qin (clock, reset, ruS_qin_d, ruS_qin_e, ruS_qin_v, ruS_qin_b, ruS_qout_d, ruS_qout_e, ruS_qout_v, ruS_qout_b);

  input  clock;
  input  reset;

  input  [15:0] ruS_qin_d;
  input  ruS_qin_e;
  input  ruS_qin_v;
  output ruS_qin_b;
  output [15:0] ruS_qout_d;
  output ruS_qout_e;
  output ruS_qout_v;
  input  ruS_qout_b;

  Q_lowqli_in #(16, 17, 0, 0, 0) q_ruS (clock, reset, {ruS_qin_d, ruS_qin_e}, ruS_qin_v, ruS_qin_b, {ruS_qout_d, ruS_qout_e}, ruS_qout_v, ruS_qout_b);

endmodule  // _page_izigzag_d1_ScOrEtMp50_qin
