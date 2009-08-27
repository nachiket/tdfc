// - Hand coded verilog for ZLE top level
// - Zero run-length encoder, no EOS cases, compiled "tdfc -xc"
// - Datapath resource sharing for outputs, registers, but NOT flags
// - No "fire" variable;  firing implicit in verilog "if" stmts

`include "zle_xc9_fsm.v"
`include "zle_xc9_dp.v"

module zle_xc9 (clock, reset,  i_d, i_v, i_b,  o_d, o_v, o_b);
   
   input  clock, reset;
   input  [2:0] i_d;	input  i_v;	output i_b;	// - input  stream i
   output [3:0] o_d;	output o_v;	input  o_b;	// - output stream o

   wire 	sel_o_d;			// - o_d mux select to datapath
   wire [1:0] 	sel_cnt;			// - cnt mux select to datapath
   wire 	f_start_i_eq_0,
		f_zeros_i_eq_0,
		f_zeros_t_cnt_eq_15;		// - flags from datapath
   
   zle_xc9_fsm fsm (clock, reset,  i_v, i_b,  o_v, o_b,  sel_o_d, sel_cnt,
		    f_start_i_eq_0, f_zeros_i_eq_0, f_zeros_t_cnt_eq_15);
   
   zle_xc9_dp  dp  (clock, reset, i_d, o_d, sel_o_d, sel_cnt, i_b,
		    f_start_i_eq_0, f_zeros_i_eq_0, f_zeros_t_cnt_eq_15);

endmodule // zle_xc9

