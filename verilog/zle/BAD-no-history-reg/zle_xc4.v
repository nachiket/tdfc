// - Hand coded verilog for ZLE top level
// - Zero run-length encoder, no EOS cases, compiled "tdfc -xc"
// - state parameters at top level only  (otherwise same as zle_xc)

`define state_start	= 4'd0;
`define state_start_t	= 4'd1;
`define state_start_e	= 4'd2;
`define state_zeros	= 4'd3;
`define state_zeros_t	= 4'd4;
`define state_zeros_t_t	= 4'd5;
`define state_zeros_t_e	= 4'd6;
`define state_zeros_e	= 4'd7;
`define state_pending	= 4'd8;

`include "zle_xc4_fsm.v"
`include "zle_xc4_dp.v"

module zle_xc4 (clock, reset,  i_d, i_v, i_b,  o_d, o_v, o_b);
   
   input  clock, reset;
   input  [2:0] i_d;	input  i_v;	output i_b;	// - input  stream i
   output [3:0] o_d;	output o_v;	input  o_b;	// - output stream o

   wire [3:0] 	state;				// - state to datapath
   wire 	f_start_i_eq_0,
		f_zeros_i_eq_0,
		f_zeros_t_cnt_eq_15;		// - flags from datapath
      
   zle_xc4_fsm fsm (clock, reset,  i_v, i_b,  o_v, o_b,  state,
		    f_start_i_eq_0, f_zeros_i_eq_0, f_zeros_t_cnt_eq_15);
   
   zle_xc4_dp  dp  (clock, reset, i_d, o_d, state,
		    f_start_i_eq_0, f_zeros_i_eq_0, f_zeros_t_cnt_eq_15);

endmodule // zle_xc4
