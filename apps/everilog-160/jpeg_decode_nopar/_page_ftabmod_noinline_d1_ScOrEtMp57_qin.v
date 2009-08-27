// Verilog page input queue module for _page_ftabmod_noinline_d1_ScOrEtMp57
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:44 2009

`include "Q_lowqli_in.v"

module _page_ftabmod_noinline_d1_ScOrEtMp57_qin (clock, reset, ind_qin_d, ind_qin_e, ind_qin_v, ind_qin_b, ind_qout_d, ind_qout_e, ind_qout_v, ind_qout_b, segment_r_data_qin_d, segment_r_data_qin_e, segment_r_data_qin_v, segment_r_data_qin_b, segment_r_data_qout_d, segment_r_data_qout_e, segment_r_data_qout_v, segment_r_data_qout_b);

  input  clock;
  input  reset;

  input  [7:0] ind_qin_d;
  input  ind_qin_e;
  input  ind_qin_v;
  output ind_qin_b;
  output [7:0] ind_qout_d;
  output ind_qout_e;
  output ind_qout_v;
  input  ind_qout_b;
  input  [63:0] segment_r_data_qin_d;
  input  segment_r_data_qin_e;
  input  segment_r_data_qin_v;
  output segment_r_data_qin_b;
  output [63:0] segment_r_data_qout_d;
  output segment_r_data_qout_e;
  output segment_r_data_qout_v;
  input  segment_r_data_qout_b;

  Q_lowqli_in #(16, 9, 0, 0, 0) q_ind (clock, reset, {ind_qin_d, ind_qin_e}, ind_qin_v, ind_qin_b, {ind_qout_d, ind_qout_e}, ind_qout_v, ind_qout_b);
  Q_lowqli_in #(16, 65, 0, 0, 0) q_segment_r_data (clock, reset, {segment_r_data_qin_d, segment_r_data_qin_e}, segment_r_data_qin_v, segment_r_data_qin_b, {segment_r_data_qout_d, segment_r_data_qout_e}, segment_r_data_qout_v, segment_r_data_qout_b);

endmodule  // _page_ftabmod_noinline_d1_ScOrEtMp57_qin
