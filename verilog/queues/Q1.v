// - Cascadable queue stage, depth 1
// - To be active on input and output,
//     stage accepts input (deasserts backpressure) only if empty

module Q1 (clock, reset, X_d, X_e, X_v, X_b, Y_d, Y_e, Y_v, Y_b);

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

   reg    [W-1:0]  r_d;		// - queue reg:	     data
   reg 		   r_e;		//		     eos
   reg 		   r_v;		//		     valid

   wire   [W-1:0]  r_d_;
   wire 	   r_e_;
   wire 	   r_v_;

   wire 	   fire;
// wire 	   emit;

   assign Y_d  = r_d;
   assign Y_e  = r_e;
   assign Y_v  = r_v;
   assign X_b  = r_v;				// - accept input iff empty
   assign fire = !r_v && X_v;
// assign emit = r_v && !Y_b;
// assign r_v_ = (r_v && !emit) || (!r_v && fire);
   assign r_v_ = (r_v && Y_b)   || fire;
   assign r_d_ = (fire ? X_d : r_d);
   assign r_e_ = (fire ? X_e : r_e);
   
   always @(posedge clock or negedge reset) begin
      if (!reset)  begin  r_d<=0;    r_e<=0;    r_v<=0;     end
      else         begin  r_d<=r_d_; r_e<=r_e_; r_v<=r_v_;  end
   end

endmodule // Q1
