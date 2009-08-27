// - Hand coded verilog for ZLE top level
// - Zero run-length encoder, no EOS cases, compiled "tdfc -xc"
// - No resource sharing
// - No "fire" variable;  firing implicit in verilog "if" stmts
// - 7->8 bit compression instead of 3->4

`include "zle_xcB_fsm.v"
`include "zle_xcB_dp.v"

module zle_xcB (clock, reset,  i_d, i_v, i_b,  o_d, o_v, o_b);
   
   input  clock, reset;
   input  [6:0] i_d;	input  i_v;	output i_b;	// - input  stream i
   output [7:0] o_d;	output o_v;	input  o_b;	// - output stream o

   wire [3:0] 	state;				// - state to datapath
   wire 	f_start_i_eq_0,
		f_zeros_i_eq_0,
		f_zeros_t_cnt_eq_127;		// - flags from datapath
   
   zle_xcB_fsm fsm (clock, reset,  i_v, i_b,  o_v, o_b,  state,
		    f_start_i_eq_0, f_zeros_i_eq_0, f_zeros_t_cnt_eq_127);
   
   zle_xcB_dp  dp  (clock, reset, i_d, o_d, state,
		    f_start_i_eq_0, f_zeros_i_eq_0, f_zeros_t_cnt_eq_127);

endmodule // zle_xcB
