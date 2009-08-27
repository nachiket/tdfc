// Verilog black box module for segment_r_0
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:43 2009

module segment_r_0 (clock, reset, addr_d, addr_e, addr_v, addr_b, data_d, data_e, data_v, data_b)  /* synthesis syn_black_box */ ;

  input  clock;
  input  reset;

  input  [31:0] addr_d;
  input  addr_e;
  input  addr_v;
  output addr_b;
  output [63:0] data_d;
  output data_e;
  output data_v;
  input  data_b;

endmodule  // segment_r_0
