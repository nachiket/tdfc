// - Hand coded verilog for ZLE top level
// - Zero run-length encoder, no EOS cases
// - No resource sharing
// - No "fire" variable;  firing implicit in verilog "if" stmts
// - Blocking assignment to overwrite state, control flow, regs, etc.

`include "zle_fsm.v"
`include "zle_dp.v"

module zle (clock, reset,  i_d, i_v, i_b,  o_d, o_v, o_b);
   
   input  clock, reset;
   input  [2:0] i_d;	input  i_v;	output i_b;	// - input  stream i
   output [3:0] o_d;	output o_v;	input  o_b;	// - output stream o

   wire   [1:0]	state;				// - state to datapath
   wire         fire;				// - fire  to datapath
   wire 	f_start_i_eq_0,
		f_zeros_i_eq_0,
		f_zeros_cnt_eq_15;		// - flags from datapath
   
   zle_fsm fsm (clock, reset,  i_v, i_b,  o_v, o_b,  state, fire,
		f_start_i_eq_0, f_zeros_i_eq_0, f_zeros_cnt_eq_15);
   
   zle_dp  dp  (clock, reset, i_d, o_d, state, fire,
		f_start_i_eq_0, f_zeros_i_eq_0, f_zeros_cnt_eq_15);

endmodule // zle
