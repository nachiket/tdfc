// - Hand coded verilog for ZLE top level
// - Zero run-length encoder, no EOS cases, compiled "tdfc -xc"
// - No resource sharing
// - Explicit "fire" signal  -->  (17% slower, +1 LUT vs. implicit fire)
// - FSM case stmt has identical "no fire" code in every case

`include "zle_xc2_fsm.v"
`include "zle_xc_dp.v"

module zle_xc2 (clock, reset,  i_d, i_v, i_b,  o_d, o_v, o_b);
   
   input  clock, reset;
   input  [2:0] i_d;	input  i_v;	output i_b;	// - input  stream i
   output [3:0] o_d;	output o_v;	input  o_b;	// - output stream o

   wire [3:0] 	state;				// - state to datapath
   wire 	f_start_i_eq_0,
		f_zeros_i_eq_0,
		f_zeros_t_cnt_eq_15;		// - flags from datapath
   
   zle_xc2_fsm fsm (clock, reset,  i_v, i_b,  o_v, o_b,  state,
		    f_start_i_eq_0, f_zeros_i_eq_0, f_zeros_t_cnt_eq_15);
   
   zle_xc_dp   dp  (clock, reset, i_d, o_d, state,
		    f_start_i_eq_0, f_zeros_i_eq_0, f_zeros_t_cnt_eq_15);

endmodule // zle_xc2
