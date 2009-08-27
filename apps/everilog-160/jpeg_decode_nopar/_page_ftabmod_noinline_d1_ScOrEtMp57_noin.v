// Verilog page module without input queues for _page_ftabmod_noinline_d1_ScOrEtMp57
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:44 2009

`include "_page_ftabmod_noinline_d1_ScOrEtMp57_q.v"
`include "ftabmod_noinline_d1_ScOrEtMp57.v"

module _page_ftabmod_noinline_d1_ScOrEtMp57_noin (clock, reset, ind_d, ind_e, ind_v, ind_b, oval_d, oval_e, oval_v, oval_b, segment_r_addr_d, segment_r_addr_e, segment_r_addr_v, segment_r_addr_b, segment_r_data_d, segment_r_data_e, segment_r_data_v, segment_r_data_b);

  input  clock;
  input  reset;

  input  [7:0] ind_d;
  input  ind_e;
  input  ind_v;
  output ind_b;
  output [7:0] oval_d;
  output oval_e;
  output oval_v;
  input  oval_b;
  output [31:0] segment_r_addr_d;
  output segment_r_addr_e;
  output segment_r_addr_v;
  input  segment_r_addr_b;
  input  [63:0] segment_r_data_d;
  input  segment_r_data_e;
  input  segment_r_data_v;
  output segment_r_data_b;

  wire   [7:0] ind_qin_d, ind_qout_d;
  wire   ind_qin_e, ind_qout_e;
  wire   ind_qin_v, ind_qout_v;
  wire   ind_qin_b, ind_qout_b;
  wire   [7:0] oval_qin_d, oval_qout_d;
  wire   oval_qin_e, oval_qout_e;
  wire   oval_qin_v, oval_qout_v;
  wire   oval_qin_b, oval_qout_b;
  wire   [31:0] segment_r_addr_qin_d, segment_r_addr_qout_d;
  wire   segment_r_addr_qin_e, segment_r_addr_qout_e;
  wire   segment_r_addr_qin_v, segment_r_addr_qout_v;
  wire   segment_r_addr_qin_b, segment_r_addr_qout_b;
  wire   [63:0] segment_r_data_qin_d, segment_r_data_qout_d;
  wire   segment_r_data_qin_e, segment_r_data_qout_e;
  wire   segment_r_data_qin_v, segment_r_data_qout_v;
  wire   segment_r_data_qin_b, segment_r_data_qout_b;

  assign ind_qout_d = ind_d;
  assign ind_qout_e = ind_e;
  assign ind_qout_v = ind_v;
  assign ind_b = ind_qout_b;
  assign oval_d = oval_qout_d;
  assign oval_e = oval_qout_e;
  assign oval_v = oval_qout_v;
  assign oval_qout_b = oval_b;
  assign segment_r_addr_d = segment_r_addr_qout_d;
  assign segment_r_addr_e = segment_r_addr_qout_e;
  assign segment_r_addr_v = segment_r_addr_qout_v;
  assign segment_r_addr_qout_b = segment_r_addr_b;
  assign segment_r_data_qout_d = segment_r_data_d;
  assign segment_r_data_qout_e = segment_r_data_e;
  assign segment_r_data_qout_v = segment_r_data_v;
  assign segment_r_data_b = segment_r_data_qout_b;

  _page_ftabmod_noinline_d1_ScOrEtMp57_q _page_ftabmod_noinline_d1_ScOrEtMp57_q_ (clock, reset, oval_qin_d, oval_qin_e, oval_qin_v, oval_qin_b, oval_qout_d, oval_qout_e, oval_qout_v, oval_qout_b, segment_r_addr_qin_d, segment_r_addr_qin_e, segment_r_addr_qin_v, segment_r_addr_qin_b, segment_r_addr_qout_d, segment_r_addr_qout_e, segment_r_addr_qout_v, segment_r_addr_qout_b);

  ftabmod_noinline_d1_ScOrEtMp57 ftabmod_noinline_d1_ScOrEtMp57_ (clock, reset, ind_qout_d, ind_qout_e, ind_qout_v, ind_qout_b, oval_qin_d, oval_qin_e, oval_qin_v, oval_qin_b, segment_r_addr_qin_d, segment_r_addr_qin_e, segment_r_addr_qin_v, segment_r_addr_qin_b, segment_r_data_qout_d, segment_r_data_qout_e, segment_r_data_qout_v, segment_r_data_qout_b);

endmodule  // _page_ftabmod_noinline_d1_ScOrEtMp57_noin
