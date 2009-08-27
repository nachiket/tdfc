// - Depth 2 queue as cascade of 2 Q1
// - Note, queue has 1/2 throughput  (I/O every other cycle)

`include "Q1.v"

module Q1x2 (clock, reset, X_d, X_e, X_v, X_b, Y_d, Y_e, Y_v, Y_b);

   parameter       W = 8;	// - data width (bits)

   input 	   clock;
   input 	   reset;
   
   input  [W-1:0]  X_d;		// - input  stream:  data
   input 	   X_e;		//		     eos
   input 	   X_v;		//		     valid
   output 	   X_b;		//		     backpressure

   output [W-1:0]  Y_d;		// - output stream:  data
   output 	   Y_e;		//		     eos
   output 	   Y_v;		//		     valid
   input 	   Y_b;		//		     backpressure

   wire   [W-1:0]  W_d;		// - intermediate stream:  data
   wire  	   W_e;		//		     eos
   wire  	   W_v;		//		     valid
   wire		   W_b;		//		     backpressure

   Q1 #(8) q1_1 (clock, reset, X_d, X_e, X_v, X_b, W_d, W_e, W_v, W_b);
   Q1 #(8) q1_2 (clock, reset, W_d, W_e, W_v, W_b, Y_d, Y_e, Y_v, Y_b);

endmodule // Q1x2
