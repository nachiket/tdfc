// Verilog page non-input queue module for _page_ftabmod_noinline_d1_ScOrEtMp57
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:44 2009

`include "Q_lowqli.v"
`include "Q_lowqli_out.v"

module _page_ftabmod_noinline_d1_ScOrEtMp57_q (clock, reset, oval_qin_d, oval_qin_e, oval_qin_v, oval_qin_b, oval_qout_d, oval_qout_e, oval_qout_v, oval_qout_b, segment_r_addr_qin_d, segment_r_addr_qin_e, segment_r_addr_qin_v, segment_r_addr_qin_b, segment_r_addr_qout_d, segment_r_addr_qout_e, segment_r_addr_qout_v, segment_r_addr_qout_b);

  input  clock;
  input  reset;

  input  [7:0] oval_qin_d;
  input  oval_qin_e;
  input  oval_qin_v;
  output oval_qin_b;
  output [7:0] oval_qout_d;
  output oval_qout_e;
  output oval_qout_v;
  input  oval_qout_b;
  input  [31:0] segment_r_addr_qin_d;
  input  segment_r_addr_qin_e;
  input  segment_r_addr_qin_v;
  output segment_r_addr_qin_b;
  output [31:0] segment_r_addr_qout_d;
  output segment_r_addr_qout_e;
  output segment_r_addr_qout_v;
  input  segment_r_addr_qout_b;

  Q_lowqli_out #(16, 9, 0, 0, 0) q_oval (clock, reset, {oval_qin_d, oval_qin_e}, oval_qin_v, oval_qin_b, {oval_qout_d, oval_qout_e}, oval_qout_v, oval_qout_b);
  Q_lowqli_out #(16, 33, 0, 0, 0) q_segment_r_addr (clock, reset, {segment_r_addr_qin_d, segment_r_addr_qin_e}, segment_r_addr_qin_v, segment_r_addr_qin_b, {segment_r_addr_qout_d, segment_r_addr_qout_e}, segment_r_addr_qout_v, segment_r_addr_qout_b);

endmodule  // _page_ftabmod_noinline_d1_ScOrEtMp57_q
