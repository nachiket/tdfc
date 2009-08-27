// Verilog top module for ftabmod_noinline_d1_ScOrEtMp57
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:44 2009

`include "ftabmod_noinline_d1_ScOrEtMp57_fsm.v"
`include "ftabmod_noinline_d1_ScOrEtMp57_dp.v"

module ftabmod_noinline_d1_ScOrEtMp57 (clock, reset, ind_d, ind_e, ind_v, ind_b, oval_d, oval_e, oval_v, oval_b, segment_r_addr_d, segment_r_addr_e, segment_r_addr_v, segment_r_addr_b, segment_r_data_d, segment_r_data_e, segment_r_data_v, segment_r_data_b);

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

  wire state;
  wire statecase;

  ftabmod_noinline_d1_ScOrEtMp57_fsm fsm (clock, reset, ind_e, ind_v, ind_b, oval_e, oval_v, oval_b, segment_r_addr_e, segment_r_addr_v, segment_r_addr_b, segment_r_data_e, segment_r_data_v, segment_r_data_b, state, statecase);
  ftabmod_noinline_d1_ScOrEtMp57_dp dp (clock, reset, ind_d, oval_d, segment_r_addr_d, segment_r_data_d, state, statecase);

endmodule  // ftabmod_noinline_d1_ScOrEtMp57
