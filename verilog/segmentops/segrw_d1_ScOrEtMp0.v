// Verilog top module for segrw_d1_ScOrEtMp0
// Emitted by ../../tdfc version 1.158, Tue Dec 14 12:18:04 2004

`include "segrw_d1_ScOrEtMp0_fsm.v"
`include "segrw_d1_ScOrEtMp0_dp.v"

module segrw_d1_ScOrEtMp0 (clock, reset, addr_d, addr_e, addr_v, addr_b, dataR_d, dataR_e, dataR_v, dataR_b, dataW_d, dataW_e, dataW_v, dataW_b, write_d, write_e, write_v, write_b);

  input  clock;
  input  reset;

  input  [3:0] addr_d;
  input  addr_e;
  input  addr_v;
  output addr_b;
  output [7:0] dataR_d;
  output dataR_e;
  output dataR_v;
  input  dataR_b;
  input  [7:0] dataW_d;
  input  dataW_e;
  input  dataW_v;
  output dataW_b;
  input  write_d;
  input  write_e;
  input  write_v;
  output write_b;

  wire state;
  wire [1:0] statecase;
  wire flag_steady_0;
  wire flag_steady_1;

  segrw_d1_ScOrEtMp0_fsm fsm (clock, reset, addr_e, addr_v, addr_b, dataR_e, dataR_v, dataR_b, dataW_e, dataW_v, dataW_b, write_e, write_v, write_b, state, statecase, flag_steady_0, flag_steady_1);
  segrw_d1_ScOrEtMp0_dp dp (clock, reset, addr_d, dataR_d, dataW_d, write_d, state, statecase, flag_steady_0, flag_steady_1);

endmodule  // segrw_d1_ScOrEtMp0
