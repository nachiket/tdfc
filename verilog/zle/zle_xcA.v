// - Hand coded verilog for ZLE top level
// - Zero run-length encoder, no EOS cases, compiled "tdfc -xc"
// - Datapath resource sharing + flag wire sharing
// - No "fire" variable;  firing implicit in verilog "if" stmts

`include "zle_xcA_fsm.v"
`include "zle_xcA_dp.v"

module zle_xcA (clock, reset,  i_d, i_v, i_b,  o_d, o_v, o_b);
   
   input  clock, reset;
   input  [2:0] i_d;	input  i_v;	output i_b;	// - input  stream i
   output [3:0] o_d;	output o_v;	input  o_b;	// - output stream o

   wire 	sel_o_d;			// - o_d mux select to datapath
   wire [1:0] 	sel_cnt;			// - cnt mux select to datapath
   wire         sel_f;				// - f   mux select to datapath
   wire 	f;				// - flags from datapath
   
   zle_xcA_fsm fsm (clock, reset,  i_v, i_b,  o_v, o_b,  sel_o_d, sel_cnt,
		    sel_f, f);
   
   zle_xcA_dp  dp  (clock, reset, i_d, o_d, sel_o_d, sel_cnt, i_b, sel_f, f);

endmodule // zle_xcA
