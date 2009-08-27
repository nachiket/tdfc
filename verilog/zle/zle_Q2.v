// - Hand coded verilog for ZLE top level
// - Zero run-length encoder, no EOS cases
// - No resource sharing
// - No "fire" variable;  firing implicit in verilog "if" stmts
// - Blocking assignment to overwrite state, control flow, regs, etc.
// - Attached to Q2 input and Q2 output

`include "zle.v"
`include "../Queues/Q2.v"

module zle_Q2 (clock, reset,  i_d, i_v, i_b,  o_d, o_v, o_b);

   parameter Wi = 2;
   parameter Wo = 3;
   
   input  clock, reset;

   input  [Wi-1:0] i_d;		// - input  stream:  data
// input 	   i_e;		//		     eos
   input 	   i_v;		//		     valid
   output 	   i_b;		//		     backpressure

   output [Wo-1:0] o_d;		// - output stream:  data
// output 	   o_e;		//		     eos
   output 	   o_v;		//		     valid
   input 	   o_b;		//		     backpressure

   wire   [Wi-1:0] Wi_d;	// - intermediate input  stream:  data
   wire  	   Wi_e;	//		     eos
   wire  	   Wi_v;	//		     valid
   wire		   Wi_b;	//		     backpressure

   wire   [Wo-1:0] Wo_d;	// - intermediate output stream:  data
   wire  	   Wo_e;	//		     eos
   wire  	   Wo_v;	//		     valid
   wire		   Wo_b;	//		     backpressure

   wire 	   i_e, o_e;	// - dummy EOS wires
   assign 	   i_e = 1'b0;
   
   Q2 #(Wi) q2_i (clock,reset, i_d,  i_e,  i_v,  i_b,  Wi_d, Wi_e, Wi_v, Wi_b);
   zle      zle  (clock,reset, Wi_d,       Wi_v, Wi_b, Wo_d,       Wo_v, Wo_b);
   Q2 #(Wo) q2_o (clock,reset, Wo_d, Wo_e, Wo_v, Wo_b, o_d,  o_e,  o_v,  o_b);

endmodule // zle_Q2
