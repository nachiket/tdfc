// - Hand coded verilog for ZLE top level
// - Zero run-length encoder, no EOS cases
// - No resource sharing
// - No "fire" variable;  firing implicit in verilog "if" stmts
// - Blocking assignment to overwrite state, control flow, regs, etc.
// - 7->8 bit compression instead of 3->4

`include "zle_B_fsm.v"
`include "zle_B_dp.v"

module zle_B (clock, reset,  i_d, i_v, i_b,  o_d, o_v, o_b);
   
   input  clock, reset;
   input  [6:0] i_d;	input  i_v;	output i_b;	// - input  stream i
   output [7:0] o_d;	output o_v;	input  o_b;	// - output stream o

   wire   [1:0]	state;				// - state to datapath
   wire         fire;				// - fire  to datapath
   wire 	f_start_i_eq_0,
		f_zeros_i_eq_0,
		f_zeros_cnt_eq_127;		// - flags from datapath
   
   zle_B_fsm fsm (clock, reset,  i_v, i_b,  o_v, o_b,  state, fire,
		  f_start_i_eq_0, f_zeros_i_eq_0, f_zeros_cnt_eq_127);
   
   zle_B_dp  dp  (clock, reset, i_d, o_d, state, fire,
		  f_start_i_eq_0, f_zeros_i_eq_0, f_zeros_cnt_eq_127);

endmodule // zle_B
